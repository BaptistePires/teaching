#include "shadow.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "memory.h"
#include "state.h"
#include "vector.h"

void set_flat_mapping(size_t ram)
{
	if (ram < GUEST_LOW_SIZE) {
		map_page(GUEST_LOW_START, GUEST_LOW_START, ram);
		return;
	}
	map_page(GUEST_LOW_START, GUEST_LOW_START, GUEST_LOW_SIZE);
}

void set_page_table(void)
{
	paddr_t cr3 = mov_from_control(3);
	parse_page_table(cr3);
}

void trap_unmap_page(paddr_t paddr)
{
	paddr_t cr3 = mov_from_control(3);
	struct shadow_table *table = find_shadow_table(cr3, 1);

	for (size_t i = 0; i < table->len; i++) {
		struct shadow_table_entry *entry = &table->entries[i];
		if (entry->paddr == paddr) {
			unmap_page(entry->vaddr, entry->size);
			fprintf(stderr,
				"trap_unmap_page: 0x%lx with paddr 0x%lx\n",
				entry->vaddr, entry->paddr);
		}
	}
}

void update_mappings(struct shadow_table *table)
{
	for (size_t i = 0; i < table->len; i++) {
		struct shadow_table_entry *entry = &table->entries[i];
		if (!GUEST_VALID(entry->vaddr))
			continue;
		unmap_page(entry->vaddr, entry->size);
		map_page(entry->vaddr, entry->paddr, entry->size);
	}
}

void protect_pagelvls(struct shadow_table *table)
{
	for (int i = 0; i < PAGE_TABLE_PADDR_ENTRIES; i++) {
		paddr_t entry = page_table_paddr[i];

		if (entry == 0)
			continue;

		if (!paddr_mapped_shadow_table(table, entry)) {
			fprintf(stderr,
				"protect_pagelvls: page table' page paddr is not mapped at 0x%lx for cr3=0x%lx\n",
				entry, table->cr3);
			abort();
		}

		mprotect((void *)entry, PAGE_SIZE, PROT_READ);
	}
}

void parse_page_table(paddr_t cr3)
{
	struct shadow_table *table = find_shadow_table(cr3, 1);
	clear_shadow_table(table);

	parse_page_pml(cr3, table, 0, 4);
	update_mappings(table);
	protect_pagelvls(table);
}

void parse_page_pml(paddr_t pml, struct shadow_table *table, vaddr_t prefix,
		    uint8_t lvl)
{
	paddr_t p[PAGE_SIZE];
	read_physical(p, PAGE_SIZE, pml);

	for (int i = 0; i < PAGE_NB_ENTRIES; i++) {
		paddr_t entry = p[i];
		paddr_t page_addr = PAGE_ADDRESS(entry);
		if (lvl != 1)
			add_entry_page_table_paddr(page_addr);

		if (PAGE_IS_VALID(entry)) {
			size_t size = 0x1 << (12 + (lvl - 1) * 9);
			prefix = prefix + i * size;

			if (PAGE_IS_HUGE(entry) || lvl == 1) {
				map_entry_shadow_table(table, prefix, page_addr,
						       size);
			} else {
				parse_page_pml(page_addr, table, prefix,
					       lvl - 1);
			}
		}
	}
}

static void guest_trigger_pgfault(vaddr_t addr)
{
	set_control(addr, 2);
	trigger_interrupt(INTERRUPT_PF, 0);
}

int trap_read(vaddr_t addr, size_t size, uint64_t *val)
{
	(void)size;
	(void)val;

	if (GUEST_HIGH(addr)) {
		fprintf(stderr, "trap_read: lazy allocation at 0x%lx\n", addr);
		guest_trigger_pgfault(addr);
		return 0;
	}

	fprintf(stderr,
		"trap_read: addr outside of high zone at 0x%lx, also known as segfault ;)\n",
		addr);
	abort();
}

int trap_write(vaddr_t addr, size_t size, uint64_t val)
{
	(void)size;

	if (GUEST_HIGH(addr)) {
		fprintf(stderr, "trap_write: lazy allocation at 0x%lx\n", addr);
		guest_trigger_pgfault(addr);
		return 1;
	}

	if (addr >= VGA_START && addr <= VGA_START + PAGE_SIZE) {
		// one entry in the vga array is 2 bytes
		write_vga((addr - VGA_START) / 2, (uint16_t)val);
		return 1;
	}

	// update the shadow page table and the guest page table
	if (paddr_in_page_table(addr + sizeof(uint64_t))) {
		if (val == 0)
			trap_unmap_page(PAGE_ADDRESS(*(uint64_t *)addr));

		mprotect((void *)(addr - (addr % PAGE_SIZE)), 10,
			 PROT_READ | PROT_WRITE);
		*(uint64_t *)addr = val;
		set_page_table();

		return 1;
	}

	fprintf(stderr, "trap_write outside vga / page table zone at %lx\n",
		addr);
	exit(EXIT_FAILURE);
}

/* Intermediate entries */

paddr_t page_table_paddr[PAGE_TABLE_PADDR_ENTRIES];

void setup_page_table_paddr()
{
	memset(page_table_paddr, 0, sizeof(page_table_paddr));
}

void display_page_table_paddr()
{
	for (int i = 0; i < PAGE_TABLE_PADDR_ENTRIES; i++) {
		paddr_t entry = page_table_paddr[i];
		if (entry == 0)
			continue;
		printf("page_table addr: 0x%lx\n", entry);
	}
}

void add_entry_page_table_paddr(paddr_t page_paddr)
{
	for (int i = 0; i < PAGE_TABLE_PADDR_ENTRIES; i++) {
		paddr_t entry = page_table_paddr[i];
		if (entry == page_paddr)
			return;
		if (entry == 0) {
			page_table_paddr[i] = page_paddr;
			return;
		}
	}

	fprintf(stderr, "add_entry_page_table_paddr: too much entries\n");
	abort();
}

int paddr_in_page_table(paddr_t paddr)
{
	for (int i = 0; i < PAGE_TABLE_PADDR_ENTRIES; i++) {
		paddr_t entry = page_table_paddr[i];
		if (paddr >= entry && paddr <= entry + PAGE_SIZE)
			return 1;
	}
	return 0;
}

/* Shadow page table */

struct shadow_table shadow_tables[SHADOW_TABLE_NB];

void setup_shadow_tables()
{
	memset(shadow_tables, 0, sizeof(shadow_tables));
}

void clear_shadow_table(struct shadow_table *table)
{
	table->len = 0;
}

void display_shadow_tables()
{
	for (int i = 0; i < SHADOW_TABLE_NB; i++) {
		struct shadow_table *table = &shadow_tables[i];
		if (table->cr3 == 0)
			break;
		fprintf(stderr, "(%d) shadow table at cr3=0x%lx\n", i,
			table->cr3);
		display_shadow_table(table->cr3);
	}
}

void display_shadow_table(paddr_t cr3)
{
	struct shadow_table *table = find_shadow_table(cr3, 0);
	for (size_t i = 0; i < table->len; i++) {
		struct shadow_table_entry *entry = &table->entries[i];
		fprintf(stderr, "v: 0x%lx, p: 0x%lx, size: 0x%lx\n",
			entry->vaddr, entry->paddr, entry->size);
	}
}

struct shadow_table *find_shadow_table(paddr_t cr3, int create)
{
	for (int i = 0; i < SHADOW_TABLE_NB; i++) {
		struct shadow_table *table = &shadow_tables[i];
		if (table->cr3 == cr3 || (create && table->cr3 == 0)) {
			if (table->cr3 == 0)
				table->cr3 = cr3;
			return table;
		}
	}

	fprintf(stderr, "find_shadow_table: no entries remaining\n");
	abort();
}

void map_entry_shadow_table(struct shadow_table *table, vaddr_t vaddr,
			    paddr_t paddr, size_t size)
{
	if (table->len == SHADOW_TABLE_ENTRIES) {
		fprintf(stderr, "map_entry_shadow_table: too much entries\n");
		abort();
	}

	for (size_t i = 0; i < table->len; i++) {
		if (table->entries[i].vaddr == vaddr)
			return;
	}

	table->entries[table->len++] = (struct shadow_table_entry){
		.paddr = paddr,
		.vaddr = vaddr,
		.size = size,
	};
}

int paddr_mapped_shadow_table(struct shadow_table *table, paddr_t paddr)
{
	for (size_t i = 0; i < table->len; i++) {
		struct shadow_table_entry *entry = &table->entries[i];
		if (paddr >= entry->paddr &&
		    paddr <= entry->paddr + entry->size)
			return 1;
	}
	return 0;
}
