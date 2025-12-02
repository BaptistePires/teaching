#ifndef _INCLUDE_SHADOW_H_
#define _INCLUDE_SHADOW_H_

#include <stdint.h>

#include "memory.h"

#define GUEST_LOW_START 0x100000
#define GUEST_LOW_END 0x80000000
#define GUEST_LOW_SIZE (GUEST_LOW_END - GUEST_LOW_START)

#define GUEST_HIGH_START 0x100000000
#define GUEST_HIGH_END 0x700000000000
#define GUEST_HIGH_SIZE (GUEST_HIGH_END - GUEST_HIGH_START)

#define GUEST_LOW(addr) ((addr) >= GUEST_LOW_START && (addr) < GUEST_LOW_END)
#define GUEST_HIGH(addr) ((addr) >= GUEST_HIGH_START && (addr) < GUEST_HIGH_END)
#define GUEST_VALID(addr) (GUEST_LOW(addr) || GUEST_HIGH(addr))

#define VGA_START 0xb8000

#define PAGE_SIZE 0x1000
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

/*
 * Memory access emulation and shadow paging.
 * Memory model for Janus VMM
 *
 * +----------------------+ 0xfffffffffffffff
 * | Monitor              |
 * | (forbidden accesses) |
 * +----------------------+ 0x700000000000
 * | Guest                |
 * +----------------------+ 0x100000000
 * | Monitor              |
 * | (forbidden accesses) |
 * +----------------------+ 0x80000000
 * | Guest                |
 * +----------------------+ 0x100000
 * | Monitor              |
 * | (trapped accesses)   |
 * +----------------------+ 0x0
 *
 * Trapped are safe to perform from the guest.
 * Forbidden accesses may succeed but would result in process corruption.
 *
 * Memory trap return code:
 *   0  - the guest should retry (typically, the memory has been mapped)
 *   !0 - the access has been emulated
 */

/* Intermediate entries */
/* Physical addresses of intermediate page table' pages */

#define PAGE_TABLE_PADDR_ENTRIES 64

extern paddr_t page_table_paddr[PAGE_TABLE_PADDR_ENTRIES];

void setup_page_table_paddr();
void display_page_table_paddr();
void add_entry_page_table_paddr(paddr_t page_paddr);
/* Check if a physical address is inside a page table' page */
int paddr_in_page_table(paddr_t paddr);

/* Shadow page tables */

#define SHADOW_TABLE_ENTRIES 64
#define SHADOW_TABLE_NB 10

struct shadow_table_entry {
	paddr_t paddr;
	vaddr_t vaddr;
	size_t size;
};

struct shadow_table {
	paddr_t cr3;
	struct shadow_table_entry entries[SHADOW_TABLE_ENTRIES];
	size_t len;
};

extern struct shadow_table shadow_tables[SHADOW_TABLE_NB];

void setup_shadow_tables();
void clear_shadow_table(struct shadow_table *table);
void display_shadow_tables();
void display_shadow_table(paddr_t cr3);
/* Find a page table with the following cr3, if none found, create a new one if create is activated */
struct shadow_table *find_shadow_table(paddr_t cr3, int create);
/* Add a mapping to a given shadow page table */
void map_entry_shadow_table(struct shadow_table *table, vaddr_t vaddr,
			    paddr_t paddr, size_t size);
/* Check if a physical address is mapped in the shadow table */
int paddr_mapped_shadow_table(struct shadow_table *table, paddr_t paddr);

void set_flat_mapping(size_t ram);

void set_page_table(void); /* install new page table in CR3 */
void trap_unmap_page(
	paddr_t paddr); /* unmap page at addr based on shadow page table */

/* Update mapping using the shadow page table, unmap old mappings */
void update_mappings(struct shadow_table *table);

/* mprotect the page table' pages physical address, verify that they are virtually mapped (kernel memory arena) */
void protect_pagelvls(struct shadow_table *table);

void parse_page_table(paddr_t cr3);
void parse_page_pml(paddr_t pml, struct shadow_table *table, vaddr_t prefix,
		    uint8_t lvl);

int trap_read(vaddr_t addr, size_t size, uint64_t *val); /* memory read */

int trap_write(vaddr_t addr, size_t size, uint64_t val); /* memory write */

#endif
