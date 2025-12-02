#ifndef _INCLUDE_MEMORY_H_
#define _INCLUDE_MEMORY_H_


#include <idt.h>
#include <task.h>
#include <types.h>

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

/**
 * Pages flags / masks
 */

#define STACK_BEGIN 0x40000000
#define STACK_END 0x2000000000

#define PAGE_SIZE 4096
#define PAGE_NB_ENTRIES 512

#define PAGE_MSK_PML4 0xff8000000000
#define PAGE_MSK_PML3 0x7fc0000000
#define PAGE_MSK_PML2 0x3fe00000
#define PAGE_MSK_PML1 0x1ff000

#define PAGE_MSK_ADDRESS 0xffffffff000

#define PAGE_FLAG_VALID 0x1
#define PAGE_FLAG_WRITE 0x2
#define PAGE_FLAG_HUGE 0x80
#define PAGE_FLAG_USER 0x4

#define PAGE_IS_VALID(p) (p & PAGE_FLAG_VALID)
#define PAGE_IS_HUGE(p) (p & PAGE_FLAG_HUGE)
#define PAGE_ADDRESS(p) (p & PAGE_MSK_ADDRESS)

#define PAGE_INDEX_PML4(p) ((p & PAGE_MSK_PML4) >> 39)
#define PAGE_INDEX_PML3(p) ((p & PAGE_MSK_PML3) >> 30)
#define PAGE_INDEX_PML2(p) ((p & PAGE_MSK_PML2) >> 21)
#define PAGE_INDEX_PML1(p) ((p & PAGE_MSK_PML1) >> 12)

paddr_t alloc_page(void);        /* Allocate a physical page identity mapped */

void free_page(paddr_t addr);  /* Release a page allocated with alloc_page() */

void print_pgt(paddr_t pml, uint8_t lvl);

void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr);

void load_task(struct task *ctx);

void set_task(struct task *ctx);

void duplicate_task(struct task *ctx);

void mmap(struct task *ctx, vaddr_t vaddr);

void munmap(struct task *ctx, vaddr_t vaddr);

void pgfault(struct interrupt_context *ctx);


#endif
