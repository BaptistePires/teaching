#include "hwdetect.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#define MEMORY_SIZE        (1ul << 21)
#define WARMUP             10000
#define PRECISION          1000000


static inline char *alloc(void)
{
	size_t i;
	char *ret = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (ret == MAP_FAILED)
		abort();

	for (i = 0; i < MEMORY_SIZE; i += PAGE_SIZE)
		ret[i] = 0;

	return ret;
}

static inline uint64_t detect(char *mem)
{
	return 0;
}

int main(void)
{
	char *mem = alloc();
	uint64_t t = detect(mem);

	printf("%d %lu\n", PARAM, t);
	return EXIT_SUCCESS;
}
