#include <idt.h>                            /* see there for interrupt names */
#include <memory.h>                               /* physical page allocator */
#include <printk.h>                      /* provides printk() and snprintk() */
#include <string.h>                                     /* provides memset() */
#include <syscall.h>                         /* setup system calls for tasks */
#include <task.h>                             /* load the task from mb2 info */
#include <types.h>              /* provides stdint and general purpose types */
#include <vga.h>                                         /* provides clear() */
#include <x86.h>                                    /* access to cr3 and cr2 */


__attribute__((noreturn))
void die(void)
{
	/* Stop fetching instructions and go low power mode */
	asm volatile ("hlt");

	/* This while loop is dead code, but it makes gcc happy */
	while (1)
		;
}

__attribute__((noreturn))
void main_multiboot2(void *mb2)
{
	clear();                                     /* clear the VGA screen */
	printk("Rackdoll OS\n-----------\n\n");                 /* greetings */

	setup_interrupts();                           /* setup a 64-bits IDT */
	setup_tss();                                  /* setup a 64-bits TSS */
	interrupt_vector[INT_PF] = pgfault;      /* setup page fault handler */

	remap_pic();               /* remap PIC to avoid spurious interrupts */
	disable_pic();                         /* disable anoying legacy PIC */
	sti();                                          /* enable interrupts */

	load_tasks(mb2);                         /* load the tasks in memory */
	run_tasks();                                 /* run the loaded tasks */

	printk("\nGoodbye!\n");                                 /* fairewell */
	die();                        /* the work is done, we can die now... */
}
