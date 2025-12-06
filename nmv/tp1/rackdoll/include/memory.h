#ifndef _INCLUDE_MEMORY_H_
#define _INCLUDE_MEMORY_H_


#include <idt.h>
#include <task.h>
#include <types.h>


#define PTE_FLAG_VALID      	0x1
#define PTE_FLAG_RW           	0x2
#define PTE_FLAG_USER      	0x4
#define PTE_FLAG_CACHED 	0x8
#define PTE_FLAG_RAM    	0x10
#define PTE_FLAG_ACCESSED      	0x20
#define PTE_FLAG_DIRTY  	0x40
#define PTE_FLAG_HUGE   	0x80
#define PTE_FLAG_GLOBAL 	0x100 // pas sur de ca
#define PTE_FLAG_NO_EXECUTE    	0x200

// pte flags masks
#define PTE_IS_VALID(p)    	((p) & PTE_FLAG_VALID)
#define PTE_IS_RW(p)      	((p) & PTE_FLAG_RW)
#define PTE_IS_USER(p)   	((p) & PTE_FLAG_USER)
#define PTE_IS_CACHED(p)	((p) & PTE_FLAG_CACHED)
#define PTE_IS_RAM(p)		((p) & PTE_FLAG_RAM)
#define PTE_IS_ACCESSED(p)	((p) & PTE_FLAG_ACCESSED)
#define PTE_IS_DIRTY(p)		((p) & PTE_FLAG_DIRTY)
#define PTE_IS_HUGE(p)		((p) & PTE_FLAG_HUGE)
#define PTE_IS_GLOBAL(p)	((p) & PTE_FLAG_GLOBAL) // pas sur de ca
#define PTE_IS_NO_EXECUTE(p)	((p) & PTE_FLAG_NO_EXECUTE)

// pte addr makss
                                
#define _PTE_ADDR_MASK		0xffffffff000
#define PTE_NEXT_ADDR(p)	((p) & _PTE_ADDR_MASK)

#define _PTE_MASK_PML1 0x1ff000
#define _PTE_MASK_PML2 0x3fe00000
#define _PTE_MASK_PML3 0x7fc0000000
#define _PTE_MASK_PML4 0xff8000000000

#define PTE_GET_INDEX_PML1(v) (((v) & _PTE_MASK_PML1) >> 12)
#define PTE_GET_INDEX_PML2(v) (((v) & _PTE_MASK_PML2) >> 21)
#define PTE_GET_INDEX_PML3(v) (((v) & _PTE_MASK_PML3) >> 30)
#define PTE_GET_INDEX_PML4(v) (((v) & _PTE_MASK_PML4) >> 39)
#define PTE_GET_INDEX_FOR_LVL(v, lvl) ((v) >> (12 + (9 * ((lvl) - 1))) & 0x1ff)


paddr_t alloc_page(void);        /* Allocate a physical page identity mapped */

void free_page(paddr_t addr);  /* Release a page allocated with alloc_page() */

void print_pgt(paddr_t pml, uint8_t level);

void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr);

void load_task(struct task *ctx);

void set_task(struct task *ctx);

void duplicate_task(struct task *ctx);

void mmap(struct task *ctx, vaddr_t vaddr);

void munmap(struct task *ctx, vaddr_t vaddr);

void pgfault(struct interrupt_context *ctx);


#endif
