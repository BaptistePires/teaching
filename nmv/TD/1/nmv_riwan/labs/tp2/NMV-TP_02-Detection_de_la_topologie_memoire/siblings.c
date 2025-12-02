#include "hwdetect.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


static inline char *alloc(size_t n)
{
	size_t i;
	char *ret = mmap(NULL, n, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (ret == MAP_FAILED)
		abort();

	for (i = 0; i < n; i += PAGE_SIZE)
		ret[i] = 0;

	return ret;
}


int main(void)
{
	return EXIT_SUCCESS;
}
