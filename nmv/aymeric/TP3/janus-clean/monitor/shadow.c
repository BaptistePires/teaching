#include "shadow.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "memory.h"
#include "state.h"
#include "vector.h"

#define GUEST_MIN_LOW 0x100000
#define GUEST_MAX_LOW 0x80000000

#define GUEST_MIN_HIGH 0x100000000
#define GUEST_MAX_HIGH 0x700000000000

#define CGA_BEGIN 0xb8000

#define PGT_VALID_MASK 0x1
#define PGT_ADDRESS_MASK 0xFFFFFFFFFF000
#define PGT_HUGEPAGE_MASK 0x80

#define PGT_IS_VALID(p)				\
	(p & PGT_VALID_MASK)
#define PGT_IS_HUGEPAGE(p)			\
	(p & PGT_HUGEPAGE_MASK)
#define PGT_ADDRESS(p)				\
	(p & PGT_ADDRESS_MASK)

#define MAX_GUEST_PROCESSES 10

struct shadow_page_table {
	paddr_t cr3;
	struct {
		vaddr_t vaddr;
		paddr_t paddr;
		size_t size;
	} mappings[64];
} shadow_page_tables[MAX_GUEST_PROCESSES];

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

struct shadow_page_table *find_shadow_page_table(paddr_t cr3, uint8_t whiten)
{
	int i;

	for (i = 0; i < MAX_GUEST_PROCESSES; ++i) {
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

void parse_pml_level(paddr_t pml, vaddr_t prefix, uint8_t lvl,
		     struct shadow_page_table *pgt, uint8_t index)
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
				pgt->mappings[index].vaddr = new_prefix;
				pgt->mappings[index].paddr = PGT_ADDRESS(p[i]);
				pgt->mappings[index].size =
					4096 << ((lvl - 1) * 9);
				index++;
			} else {
				/* Pas encore niveau final */
				parse_pml_level(PGT_ADDRESS(p[i]), new_prefix,
						lvl - 1, pgt, index);
			}
		}
	}

	free(p);
}

void parse_page_table(paddr_t cr3)
{
	struct shadow_page_table *cur = find_shadow_page_table(cr3, 1);

	parse_pml_level(cr3, 0, 4, cur, 0);
}

void set_page_table(void)
{
	paddr_t cr3 = mov_from_control(3);

	parse_page_table(cr3);
}

int trap_read(vaddr_t addr, size_t size, uint64_t *val)
{
	display_mapping();
	fprintf(stderr, "trap_read unimplemented at %lx\n", addr);
	display_vga();
	exit(EXIT_FAILURE);
}

int trap_write(vaddr_t addr, size_t size, uint64_t val)
{

	if ((addr >= CGA_BEGIN) && (addr + size) < CGA_BEGIN + 4096) {
		write_vga((addr - CGA_BEGIN) / 2, (uint16_t) val);
	}

	/* else { */

	/*	display_mapping(); */
	/*	fprintf(stderr, "trap_write unimplemented at %lx\n", addr); */
	/*	exit(EXIT_FAILURE); */
	/* } */

	/* if (val == 1891) */
	/*	display_vga(); */

	return 1;
}
