#include "shadow.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <errno.h>

#include "memory.h"
#include "state.h"
#include "vector.h"

#define GUEST_MIN_LOW 0x100000
#define GUEST_MAX_LOW 0x80000000

#define GUEST_MIN_HIGH 0x100000000
#define GUEST_MAX_HIGH 0x700000000000

#define VALID_GUEST_ACCESS(v) \
	((v >= GUEST_MIN_LOW && v < GUEST_MAX_LOW) ||	\
	 (v >= GUEST_MIN_HIGH && v < GUEST_MAX_HIGH))

#define CGA_BEGIN 0xb8000

#define PGT_VALID_MASK 0x1
#define PGT_ADDRESS_MASK 0xFFFFFFFFFF000
#define PGT_HUGEPAGE_MASK 0x80

#define PGT_IS_VALID(p) \
	(p & PGT_VALID_MASK)

#define PGT_IS_HUGEPAGE(p) \
	(p & PGT_HUGEPAGE_MASK)

#define PGT_ADDRESS(p) \
	(p & PGT_ADDRESS_MASK)

#define SHADOW_PAGE_TABLE_SIZE 10

#define MAPPING_CONTAINS(candidate, start, size)	\
	((candidate >= start) && (candidate < start + size))

#define CONTAINING_PAGE(v)			\
	((v / 4096) * 4096)

struct shadow_page_table {
	paddr_t cr3;
	struct {
		vaddr_t vaddr;
		paddr_t paddr;
		size_t size;
	} mappings[64];
	paddr_t pmlx[64];
	uint8_t mappings_index, pmlx_index;
} shadow_page_tables[SHADOW_PAGE_TABLE_SIZE];

void set_flat_mapping(size_t ram)
{

	if (ram < GUEST_MAX_LOW - GUEST_MIN_LOW) {
		map_page(GUEST_MIN_LOW, GUEST_MIN_LOW, ram);
	} else {
		map_page(GUEST_MIN_LOW, GUEST_MIN_LOW,
			 GUEST_MAX_LOW - GUEST_MIN_LOW);
		map_page(GUEST_MIN_HIGH, GUEST_MIN_HIGH,
			 ram - (GUEST_MAX_LOW - GUEST_MIN_LOW));
	}
}

void write_mapping_spt(struct shadow_page_table *spt, vaddr_t vaddr,
		       paddr_t paddr, size_t size)
{
	spt->mappings[spt->mappings_index].vaddr = vaddr;
	spt->mappings[spt->mappings_index].paddr = paddr;
	spt->mappings[spt->mappings_index].size = size;

	spt->mappings_index++;
}

void write_pmlx_spt(struct shadow_page_table *spt, paddr_t paddr)
{
	spt->pmlx[spt->pmlx_index] = paddr;

	spt->pmlx_index++;
}

void parse_pml_level(paddr_t pml, vaddr_t prefix, uint8_t lvl,
		     struct shadow_page_table *spt)
{
	uint64_t *p = malloc(4096);
	vaddr_t new_prefix;
	int i;

	read_physical(p, 4096, pml);

	for (i = 0; i < 512; i++) {
		if (PGT_IS_VALID(p[i])) {
			new_prefix = prefix + (i << (12 + (lvl - 1) * 9));
			if (lvl == 1 || PGT_IS_HUGEPAGE(p[i])) {
				/* Niveau final */
				write_mapping_spt(spt, new_prefix,
						  PGT_ADDRESS(p[i]),
						  4096 << ((lvl - 1) * 9));
			} else {
				/* Pas encore niveau final */
				write_pmlx_spt(spt, PGT_ADDRESS(p[i]));
				parse_pml_level(PGT_ADDRESS(p[i]), new_prefix,
						lvl - 1, spt);
			}
		}
	}

	free(p);
}

void protect_pagelvls(struct shadow_page_table *spt)
{
	int i, j;
	int retval;
	int offset;

	for (i = 0; i < 64; ++i) {
		for (j = 0; j < 64; ++j) {
			if (spt->pmlx[i] &&
			    spt->mappings[j].size &&
			    MAPPING_CONTAINS(spt->pmlx[i],
					     spt->mappings[j].paddr,
					     spt->mappings[j].size)) {

				/* Calcul de l'offset */
				offset = spt->pmlx[i] - spt->mappings[j].paddr;
				retval = mprotect((void *)
						  (spt->mappings[j].vaddr + offset),
						  4096,
						  PROT_READ);


			}
		}
	}
}

struct shadow_page_table *find_shadow_page_table(paddr_t cr3, uint8_t whiten)
{
	int i;

	for (i = 0; i < SHADOW_PAGE_TABLE_SIZE; ++i) {
		if (shadow_page_tables[i].cr3 == cr3 ||
		    shadow_page_tables[i].cr3 == 0) {
			if (whiten)
				memset(&shadow_page_tables[i], 0,
				       sizeof(struct shadow_page_table));
			shadow_page_tables[i].cr3 = cr3;
			return &shadow_page_tables[i];
		}
	}

	/* Ne saurait advenir */
	return NULL;
}

void update_mappings(paddr_t cr3)
{
	struct shadow_page_table *cur = find_shadow_page_table(cr3, 0);
	int i;

	if (!cur)
		return;

	for (i = 0; i < 64; ++i) {
		if (cur->pmlx[i]) {
			printf("0x%lx 0x%lx\n", cr3, cur->pmlx[i]);
		}
	}

	for (i = 0; i < 64; ++i) {
		if (cur->mappings[i].size &&
		    VALID_GUEST_ACCESS(cur->mappings[i].vaddr)) {

			unmap_page(cur->mappings[i].vaddr,
				   cur->mappings[i].size);

			/* printf("0x%lx 0x%lx %ld Kio\n", */
			/*        cur->mappings[i].vaddr, */
			/*        cur->mappings[i].paddr, */
			/*        cur->mappings[i].size / 1024); */

			map_page(cur->mappings[i].vaddr,
				 cur->mappings[i].paddr,
				 cur->mappings[i].size);


		}
	}
}

void parse_page_table(paddr_t cr3)
{
	struct shadow_page_table *spt = find_shadow_page_table(cr3, 1);

	parse_pml_level(cr3, 0, 4, spt);
}

void set_page_table(void)
{
	paddr_t cr3 = mov_from_control(3);

	parse_page_table(cr3);

	update_mappings(cr3);

	protect_pagelvls(find_shadow_page_table(cr3, 0));

	display_mapping();

	printf("\n\n");

	/* display_vga(); */
}

int trap_read(vaddr_t addr, size_t size, uint64_t *val)
{
	display_mapping();
	printf("trap_read unimplemented at %lx\n", addr);
	display_vga();
	exit(EXIT_FAILURE);
}

int trap_write(vaddr_t addr, size_t size, uint64_t val)
{
	int retval;

	if (addr >= CGA_BEGIN && addr < CGA_BEGIN + 4096) {
		write_vga((addr - CGA_BEGIN) / 2, (uint16_t) val);
	} else {
		/* Addresse de table des pages invitée */

		retval = mprotect((void *)CONTAINING_PAGE(addr),
				  4096, PROT_READ | PROT_WRITE);

		memcpy((void *)addr, &val, size);

		set_page_table();

	}

	/* if (val == 1891) */
	/*	display_vga(); */


	return 1;
}
