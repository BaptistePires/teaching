#include <idt.h>                            /* see there for interrupt names */
#include <memory.h>                               /* physical page allocator */
#include <printk.h>                      /* provides printk() and snprintk() */
#include <string.h>                                     /* provides memset() */
#include <syscall.h>                         /* setup system calls for tasks */
#include <task.h>                             /* load the task from mb2 info */
#include <types.h>              /* provides stdint and general purpose types */
#include <vga.h>                                         /* provides clear() */
#include <x86.h>                                    /* access to cr3 and cr2 */
#include <pgt.h>                                       /* Provides PGT masks */


__attribute__((noreturn))
void die(void)
{
	/* Stop fetching instructions and go low power mode */
	asm volatile ("hlt");

	/* This while loop is dead code, but it makes gcc happy */
	while (1)
		;
}

void print_pgt(paddr_t pml, uint8_t lvl)
{
	uint64_t *p;
	int i;
	p = (uint64_t *)pml;

	if (lvl == 4)
		printk("cr3 : 0x%lx\n", pml);

	if (lvl < 1)
		return;

	for (i = 0; i < 512; ++i) {
		if (PGT_IS_VALID(p[i])) {
			printk("pml%d %d : 0x%lx\n", lvl, i, PGT_ADDRESS(p[i]));
			if (!PGT_IS_HUGEPAGE(p[i])) {
				print_pgt((paddr_t) PGT_ADDRESS(p[i]), lvl - 1);
			}
		}

		/* if (PGT_IS_VALID(p[i]) && PGT_IS_HUGEPAGE(p[i])) { */
		/*	printk("pml%d %d : is a huge page\n", lvl, i, p[i] & PGT_ADDRESS_MASK); */
		/* } */

		/* if (PGT_IS_VALID(p[i]) && !PGT_IS_HUGEPAGE(p[i])) { */
		/*	printk("pml%d %d : is a normal page\n", lvl, i, p[i] & PGT_ADDRESS_MASK); */
		/* } */

	}

	/* for (i = 0; i < 512; ++i) { */
	/*	if (PGT_IS_VALID(p[i]) && !PGT_IS_HUGEPAGE(p[i])) { */
	/*		print_pgt((paddr_t) PGT_ADDRESS(p[i]), lvl - 1); */
	/*	} */
	/* } */
}

__attribute__((noreturn))
void main_multiboot2(void *mb2)
{
	/* Vérification exo 2 */
	struct task fake;
	paddr_t new;

	clear();                                     /* clear the VGA screen */
	printk("Rackdoll OS\n-----------\n\n");                 /* greetings */

	setup_interrupts();                           /* setup a 64-bits IDT */
	setup_tss();                                  /* setup a 64-bits TSS */
	interrupt_vector[INT_PF] = pgfault;      /* setup page fault handler */

	remap_pic();               /* remap PIC to avoid spurious interrupts */
	disable_pic();                         /* disable anoying legacy PIC */
	sti();                                          /* enable interrupts */

	/* uint64_t cr3 = store_cr3(); */

	/* print_pgt(cr3, 4); */

	/* Vérification exo 2 */
	fake.pgt = store_cr3();
	new = alloc_page();
	map_page(&fake, 0x201000, new);
	/* munmap(&fake, 0x201000); */


	load_tasks(mb2);                         /* load the tasks in memory */
	run_tasks();                                 /* run the loaded tasks */

	printk("\nGoodbye!\n");                                  /* farewell */
	die();                        /* the work is done, we can die now... */
}
