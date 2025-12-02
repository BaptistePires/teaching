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
	Reecrire en prenant en compte le dernier lvl, pas propre atm, le cas ou
	l'@ est deja mappee est pas pris en compte
*/
void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr)
{
	
	paddr_t *pgt_addr = (paddr_t *)ctx->pgt;
	paddr_t current_index;

	/* Pour tous les lvl intermediaires */
	for(uint8_t level = 4; level > 0; level--) {
		/* Calcul de l'index pour la pml courrante */
		current_index = PTE_GET_INDEX_FOR_LVL(vaddr, level);

		/* 
		 * La page n'est pas encore allouee, deux cas possibles :
		 * - Soit on est en pml4, pml3 ou pml2 : on alloue une nouvelle page de pgt
		 * - Soit on est en pml1 : on mappe la page physique demandee
		*/
		if (!PTE_IS_VALID(pgt_addr[current_index])) {
			paddr_t new_page = level == 1 ? paddr : alloc_page();
			memset((void *)new_page, 0, PAGE_SIZE);
			pgt_addr[current_index] = new_page | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;
		}

		/* On descend d'un niveau : risque de segfault ou pas ?*/
		pgt_addr = (paddr_t *)PTE_NEXT_ADDR(pgt_addr[current_index]);	
	}
}

void load_task(struct task *ctx)
{
}

void set_task(struct task *ctx)
{
}

void mmap(struct task *ctx, vaddr_t vaddr)
{
}

void munmap(struct task *ctx, vaddr_t vaddr)
{
}

void pgfault(struct interrupt_context *ctx)
{
	printk("Page fault at %p\n", ctx->rip);
	printk("  cr2 = %p\n", store_cr2());
	asm volatile("hlt");
}

void duplicate_task(struct task *ctx)
{
}
