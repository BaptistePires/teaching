#include "task.h"
#include "types.h"
#include <memory.h>
#include <printk.h>
#include <string.h>
#include <x86.h>

#define PHYSICAL_POOL_PAGES 64
#define PHYSICAL_POOL_BYTES (PHYSICAL_POOL_PAGES << 12)
#define BITSET_SIZE (PHYSICAL_POOL_PAGES >> 6)
#define PGT_NR_ENTRIES	512
#define PAGE_SIZE 0x1000

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
			return (((64 * i) + j) << 12) + ((paddr_t)&pool);
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

	tmp = tmp - ((paddr_t)&pool);
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



#define USER_STACK_START 0x2000000000
#define USER_STACK_END 0x40000000
/*
 * Memory model for Rackdoll OS
 *
 * +----------------------+ 0xffffffffffffffff
 * | Higher half          |
 * | (unused)             |
 * +----------------------+ 0xffff800000000000
 * | (impossible address) |
 * +----------------------+ 0x00007fffffffffff 128 TiB
 * | User                 |
 * | (text + data + heap) |
 * +----------------------+ 0x2000000000 128 GiB
 * | User                 |
 * | (stack)              | 0x1ffffffff8
 * +----------------------+ 0x40000000 1GiB
 * | Kernel               |
 * | (valloc)             |
 * +----------------------+ 0x201000  ~ +2 MiB
 * | Kernel               |
 * | (APIC)               |
 * +----------------------+ 0x200000 2 MiB
 * | Kernel               |
 * | (text + data)        |
 * +----------------------+ 0x100000  1 MiB
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
void print_pgt(paddr_t pml, uint8_t level)
{
	paddr_t *current_pml = (paddr_t *)pml;
	paddr_t pte_entry, next_pml_addr;

	if (level == 4) {
		printk("Initial pgt addr=%p, first_entry=%p\n", current_pml, current_pml[0]);
	}

	if (level == 0)
		return;

	/* Pour toutes les entrees de pmlX */
	for (uint16_t i = 0; i < PGT_NR_ENTRIES; i++) {
		
		/* Entree courante */
		pte_entry = current_pml[i];
		
		/* Si l'entrée dans la pgt n'est pas valide on continue */
		if (!PTE_IS_VALID(pte_entry))
			continue;

		/* On recupere l'@ de la next */
		next_pml_addr = PTE_NEXT_ADDR(pte_entry);

		for (uint8_t l = level; l < 4 ; l++)
			printk("\t");

		printk("level=%u, index=%u, flags=%3lx, next_addr=%p, hugepage=%u\n", level, i, pte_entry & 0xfff, next_pml_addr, !!PTE_IS_HUGE(pte_entry));
		
		if (!PTE_IS_HUGE(pte_entry))
			print_pgt(next_pml_addr, level - 1);
	}
}

/*
*/
void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr)
{
	
	paddr_t *pgt_addr = (paddr_t *)ctx->pgt;
	paddr_t current_index;

	/* Pour tous les lvl intermediaires */
	for(uint8_t level = 4; level > 1; level--) {
		/* Calcul de l'index pour la pml courrante */
		current_index = PTE_GET_INDEX_FOR_LVL(vaddr, level);

		/* 
		 * La page n'est pas encore allouee, deux cas possibles :
		 * - Soit on est en pml4, pml3 ou pml2 : on alloue une nouvelle page de pgt
		 * - Soit on est en pml1 : on mappe la page physique demandee
		*/
		if (!PTE_IS_VALID(pgt_addr[current_index])) {
			paddr_t new_page = alloc_page();
			memset((void *)new_page, 0, PAGE_SIZE);
			pgt_addr[current_index] = new_page | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;
		}

		/* On descend d'un niveau : risque de segfault ou pas ?*/
		pgt_addr = (paddr_t *)PTE_NEXT_ADDR(pgt_addr[current_index]);	
	}

	current_index = PTE_GET_INDEX_PML1(vaddr);
	if (!PTE_IS_VALID(pgt_addr[current_index])) {
		pgt_addr[current_index] = paddr | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;
	} else {
		printk("[warning] map_page: vaddr %p is already mapped\n", vaddr);
		asm volatile ("hlt");
	}
}

void load_task(struct task *ctx)
{
	/* On se trouve dans une nouvelle tache, il faut allouer pgt */
	paddr_t new_pml4 = alloc_page();
	memset((void *)new_pml4, 0, PAGE_SIZE);
	ctx->pgt = new_pml4;

	/* Pour mapper noyau, on a besoin de creer pml3
	 * car on a juste de copier pml3[0] du parent
	 * TODO check user
	*/
	paddr_t pml3 = alloc_page();
	memset((void *)pml3, 0, PAGE_SIZE);
	((paddr_t *)new_pml4)[0] = (paddr_t)pml3 | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;

	/* A partir de la, on a new_pml4[0] -> new_pml3[0], 
	 * on peut copier la pml2 du kernel.
	*/
	/* On recupere la pgt du processus courrant */
	paddr_t *kernel_pml4 = (paddr_t *)store_cr3();
	paddr_t *kernel_pml3 = (paddr_t *)PTE_NEXT_ADDR(kernel_pml4[0]);
	((paddr_t *)pml3)[0] = kernel_pml3[0];

	/* La partie setup pgt est terminee, il faut maintenant
	 * allouer.
	*/

	/* On itere sur toutes la partie load_end_paddr - load_paddr */
	vaddr_t vaddr = ctx->load_vaddr;
	paddr_t paddr = ctx->load_paddr;

	for(; paddr < ctx->load_end_paddr; paddr+=PAGE_SIZE) {
		map_page(ctx, vaddr, paddr);
		vaddr+=PAGE_SIZE;
	}


	printk("before bss alloc\n");
	/* A ce moment, vaddr = bss_start */
	for(; vaddr < ctx->bss_end_vaddr; vaddr+=PAGE_SIZE) {
		paddr_t new_page = alloc_page();
		memset((void *)new_page, 0, PAGE_SIZE);
		map_page(ctx, vaddr, new_page);
	}
	

	printk("!!Task loaded: load_vaddr=%p, load_end_vaddr=%p, bss_end_vaddr=%p\n",
		ctx->load_vaddr, vaddr,  ctx->bss_end_vaddr);
}

void set_task(struct task *ctx)
{
	load_cr3(ctx->pgt);
}

void mmap(struct task *ctx, vaddr_t vaddr)
{
	paddr_t new_page = alloc_page();
	memset((void *)new_page, 0, PAGE_SIZE);
	map_page(ctx, vaddr, new_page);
}

void munmap(struct task *ctx, vaddr_t vaddr)
{
	paddr_t *pgt = (paddr_t *)ctx->pgt;

	/* On va jusqu'a PML1 */
	for (uint8_t level = 4; level > 1; level--) {
		uint16_t index = PTE_GET_INDEX_FOR_LVL(vaddr, level);
		if (!PTE_IS_VALID(pgt[index])) {
			printk("[warning] munmap: vaddr %p is not mapped\n", vaddr);
			return;
		}
		pgt = (paddr_t *)PTE_NEXT_ADDR(pgt[index]);
	}

	// On est arrive a PML1
	uint16_t index = PTE_GET_INDEX_PML1(vaddr);
	if (!PTE_IS_VALID(pgt[index])) {
		printk("[warning] munmap: vaddr %p is not mapped\n", vaddr);
		return;
	}
	free_page(PTE_NEXT_ADDR(pgt[index]));
	pgt[index] = 0;
	invlpg(vaddr);
}

void pgfault(struct interrupt_context *ctx)
{
	paddr_t faulty_addr = store_cr2();
	
	/* Seules les fautes de page dans la pile sont valides.*/
	if (faulty_addr > USER_STACK_START || faulty_addr < USER_STACK_END) {
		exit_task(ctx);
		return;
	}
	mmap(current(), faulty_addr);
}

void duplicate_task(struct task *ctx)
{
}
