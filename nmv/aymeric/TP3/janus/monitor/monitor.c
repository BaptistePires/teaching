#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "intercept.h"
#include "memory.h"
#include "multiboot2.h"
#include "shadow.h"
#include "state.h"


#define PHYSICAL_MEMORY   (3ul << 30)


static const char *progname = "janus";

static void error(const char *format, ...)
{
	va_list ap;

	fprintf(stderr, "Error: ");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fprintf(stderr, "\nUsage: %s <kernel>\n", progname);
	exit(EXIT_FAILURE);
}

void routine(int signal)
{
	display_vga();
}

int main(int argc, const char **argv)
{
	struct multiboot2_load_info info;
	void *entry;

	/* Redéfinition routine de traitement */
	struct sigaction handler;
	handler.sa_handler = routine;
	handler.sa_flags = SA_RESETHAND;
	sigaction(7, &handler, NULL);

	progname = argv[0];

	if (argc != 2)
		error("missing kernel operand");

	allocate_physical_memory(PHYSICAL_MEMORY);
	set_flat_mapping(PHYSICAL_MEMORY);

	if (parse_multiboot2(&info, argv[1]) != 0)
		error("invalid kernel operand");

	if (load_multiboot2(&info, argv[1]) != 0)
		error("unable to load kernel");

	setup_state();
	setup_interception();

	entry = (void *) ((uint64_t) info.entry_addr);
	protect_low_memory();
	goto *entry;

	/* dead code */
	return EXIT_SUCCESS;
}
