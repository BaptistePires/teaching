#include <memory.h>
#include <printk.h>
#include <string.h>
#include <x86.h>
#include <pgt.h>


#define PHYSICAL_POOL_PAGES  64
#define PHYSICAL_POOL_BYTES  (PHYSICAL_POOL_PAGES << 12)
#define BITSET_SIZE          (PHYSICAL_POOL_PAGES >> 6)


extern __attribute__((noreturn)) void die(void);

static uint64_t bitset[BITSET_SIZE];

static uint8_t pool[PHYSICAL_POOL_BYTES] __attribute__((aligned(0x1000)));


paddr_t alloc_page(void)
{
	size_t i, j;
	uint64_t v;

	for (i = 0; i < BITSET_SIZE; i++) {
		if (bitset[i] == 0xffffffffffffffff)
			continue;

		for (j = 0; j < 64; j++) {
			v = 1ul << j;
			if (bitset[i] & v)
				continue;

			bitset[i] |= v;
			return (((64 * i) + j) << 12) + ((paddr_t) &pool);
		}
	}

	printk("[error] Not enough identity free page\n");
	return 0;
}

void free_page(paddr_t addr)
{
	paddr_t tmp = addr;
	size_t i, j;
	uint64_t v;

	tmp = tmp - ((paddr_t) &pool);
	tmp = tmp >> 12;

	i = tmp / 64;
	j = tmp % 64;
	v = 1ul << j;

	if ((bitset[i] & v) == 0) {
		printk("[error] Invalid page free %p\n", addr);
		die();
	}

	bitset[i] &= ~v;
}


/*
 * Memory model for Rackdoll OS
 *
 * +----------------------+ 0xffffffffffffffff
 * | Higher half          |
 * | (unused)             |
 * +----------------------+ 0xffff800000000000
 * | (impossible address) |
 * +----------------------+ 0x00007fffffffffff
 * | User                 |
 * | (text + data + heap) |
 * +----------------------+ 0x2000000000
 * | User                 |
 * | (stack)              |
 * +----------------------+ 0x40000000
 * | Kernel               |
 * | (valloc)             |
 * +----------------------+ 0x201000
 * | Kernel               |
 * | (APIC)               |
 * +----------------------+ 0x200000
 * | Kernel               |
 * | (text + data)        |
 * +----------------------+ 0x100000
 * | Kernel               |
 * | (BIOS + VGA)         |
 * +----------------------+ 0x0
 *
 * This is the memory model for Rackdoll OS: the kernel is located in low
 * addresses. The first 2 MiB are identity mapped and not cached.
 * Between 2 MiB and 1 GiB, there are kernel addresses which are not mapped
 * with an identity table.
 * Between 1 GiB and 128 GiB is the stack addresses for user processes growing
 * down from 128 GiB.
 * The user processes expect these addresses are always available and that
 * there is no need to map them explicitely.
 * Between 128 GiB and 128 TiB is the heap addresses for user processes.
 * The user processes have to explicitely map them in order to use them.
 */


void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr)
{
	uint64_t *p = (uint64_t *)ctx->pgt;
	paddr_t tmp;

	/* printk("\n0x%lx 0x%lx 0x%lx 0x%lx\n", PGT_PML4_INDEX(vaddr), */
	/*        PGT_PML3_INDEX(vaddr), */
	/*        PGT_PML2_INDEX(vaddr), */
	/*        PGT_PML1_INDEX(vaddr)); */

	/* PML4 */
	if (!PGT_IS_VALID(p[PGT_PML4_INDEX(vaddr)])) {
		/* printk("Entry is invalid\n"); */

		tmp = alloc_page(); /* TODO : handle error */

		memset((void *)tmp, 0, PAGE_SIZE);
		p[PGT_PML4_INDEX(vaddr)] |= (tmp | PGT_VALID_MASK);

	} else {
		/* printk("Entry is valid\n"); */
	}

	p = (uint64_t *)PGT_ADDRESS(p[PGT_PML4_INDEX(vaddr)]);

	/* printk("0x%lx\n", p); */

	/* PML3 */
	if (!PGT_IS_VALID(p[PGT_PML3_INDEX(vaddr)])) {
		/* printk("Entry is invalid\n"); */

		tmp = alloc_page(); /* TODO : handle error */

		memset((void *)tmp, 0, PAGE_SIZE);
		p[PGT_PML3_INDEX(vaddr)] |= (tmp | PGT_VALID_MASK);
	} else {
		/* printk("Entry is valid\n"); */
	}

	p = (uint64_t *)PGT_ADDRESS(p[PGT_PML3_INDEX(vaddr)]);

	/* printk("0x%lx\n", p); */

	/* PML2 */
	if (!PGT_IS_VALID(p[PGT_PML2_INDEX(vaddr)])) {
		/* printk("Entry is invalid\n"); */

		tmp = alloc_page(); /* TODO : handle error */

		memset((void *)tmp, 0, PAGE_SIZE);
		p[PGT_PML2_INDEX(vaddr)] |= (tmp | PGT_VALID_MASK);
	} else {
		/* printk("Entry is valid\n"); */
	}

	p = (uint64_t *)PGT_ADDRESS(p[PGT_PML2_INDEX(vaddr)]);

	/* printk("0x%lx\n", p); */

	/* PML1 */
	if (!PGT_IS_VALID(p[PGT_PML1_INDEX(vaddr)])) {
		/* printk("Mapping virtual address 0x%lx onto physical address 0x%lx\n", */
		/*        vaddr, paddr); */

		p[PGT_PML1_INDEX(vaddr)] |= (paddr | PGT_VALID_MASK |
					     PGT_WRITABLE_MASK | PGT_USER_MASK);
	} else {
		/* printk("[error] Virtual address 0x%lx already mapped\n", vaddr); */
	}
}

void load_task(struct task *ctx)
{
	uint64_t *p;
	paddr_t paddr;
	vaddr_t vaddr;

	/* Adresses noyau faites manuellement */
	paddr = alloc_page();
	memset((void *)paddr, 0, PAGE_SIZE);
	ctx->pgt = paddr;

	p = (uint64_t *)ctx->pgt;
	paddr = alloc_page();
	memset((void *)paddr, 0, PAGE_SIZE);
	p[0] |= (paddr | PGT_VALID_MASK | PGT_WRITABLE_MASK | PGT_USER_MASK);

	p = (uint64_t *)store_cr3();
	paddr = PGT_ADDRESS(*(uint64_t *)PGT_ADDRESS(*p));
	p = (uint64_t *)PGT_ADDRESS(*((uint64_t *)ctx->pgt));
	p[0] |= (paddr | PGT_VALID_MASK | PGT_WRITABLE_MASK | PGT_USER_MASK);

	/* Adresses payload */
	for (paddr = ctx->load_paddr; paddr < ctx->load_end_paddr; paddr += PAGE_SIZE) {
		vaddr = ctx->load_vaddr + paddr - ctx->load_paddr;
		/* printk("%lx\n", vaddr); */
		/* printk("%lx\n", paddr); */
		map_page(ctx, vaddr, paddr);
	}

	/* Adresses bss */
	for (vaddr = ctx->load_vaddr + ctx->load_end_paddr - ctx->load_paddr;
	     vaddr < ctx->bss_end_vaddr; vaddr += PAGE_SIZE) {
		paddr = alloc_page();
		/* printk("%lx\n", vaddr); */
		/* printk("%lx\n", paddr); */
		memset((void *)paddr, 0, PAGE_SIZE);
		map_page(ctx, vaddr, paddr);
	}
}

void set_task(struct task *ctx)
{
	load_cr3(ctx->pgt);
}

void mmap(struct task *ctx, vaddr_t vaddr)
{
	paddr_t paddr = alloc_page();

	memset((void *)paddr, 0, PAGE_SIZE);
	map_page(ctx, vaddr, paddr);
}

void munmap(struct task *ctx, vaddr_t vaddr)
{
	uint64_t *p4, *p3, *p2, *p1;
	paddr_t paddr;
	int i, c;

	p4 = (uint64_t *)ctx->pgt;
	p3 = (uint64_t *)PGT_ADDRESS(p4[PGT_PML4_INDEX(vaddr)]);
	p2 = (uint64_t *)PGT_ADDRESS(p3[PGT_PML3_INDEX(vaddr)]);
	p1 = (uint64_t *)PGT_ADDRESS(p2[PGT_PML2_INDEX(vaddr)]);
	paddr = PGT_ADDRESS(p1[PGT_PML1_INDEX(vaddr)]);

	free_page(paddr);
	PGT_INVALIDATE(p1[PGT_PML1_INDEX(vaddr)]);

	c = 0;
	for (i = 0; i < 512; ++i)
		   if (PGT_IS_VALID(p1[i]))
			++c;

	if (!c) {
		memset((void *)p1, 0, PAGE_SIZE);
		free_page((paddr_t)p1);
		PGT_INVALIDATE(p2[PGT_PML2_INDEX(vaddr)]);
	}

	c = 0;
	for (i = 0; i < 512; ++i)
		if (PGT_IS_VALID(p2[i]))
			++c;

	if (!c) {
		memset((void *)p2, 0, PAGE_SIZE);
		free_page((paddr_t)p2);
		PGT_INVALIDATE(p3[PGT_PML3_INDEX(vaddr)]);
	}

	c = 0;
	for (i = 0; i < 512; ++i)
		if (PGT_IS_VALID(p3[i]))
			++c;

	if (!c) {
		memset((void *)p3, 0, PAGE_SIZE);
		free_page((paddr_t)p3);
		PGT_INVALIDATE(p4[PGT_PML4_INDEX(vaddr)]);
	}

	/* On ne va pas enlever le PML4 : on a toujours les adresses noyau */

	/* On invalide l'entrée du TLB idoine */
	invlpg(vaddr);
}

void pgfault(struct interrupt_context *ctx)
{
	vaddr_t vaddr = store_cr2();
	paddr_t paddr;

	if (ADDRESS_FROM_STACK(vaddr)) {
		paddr = alloc_page();
		memset((void *)paddr, 0, PAGE_SIZE);
		map_page(current(), PAGE_BELOW(vaddr), paddr);
	} else {
		printk("Page fault at %p\n", ctx->rip);
		printk("  cr2 = %p\n", vaddr);
		exit_task(ctx);
	}
}

void duplicate_task(struct task *ctx)
{
}
