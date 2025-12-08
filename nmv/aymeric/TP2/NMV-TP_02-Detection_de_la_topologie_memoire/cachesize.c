#include "hwdetect.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#ifndef PARAM
#define PARAM 64
#endif

#define CACHELINE_SIZE     64
#define MEMORY_SIZE        (1ul << 25)
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
	size_t i, p;
	uint64_t start, end;

	setcore(0);

	for (p = 0; p < WARMUP; p++)
		for (i = 0; i < PARAM; i += CACHELINE_SIZE)
			writemem(mem + i);

	start = now();

	for (p = 0; p < PRECISION; p++)
		for (i = 0; i < PARAM; i += CACHELINE_SIZE) {
			writemem(mem + i);
			/* a++; */
		}

	end = now();

	/* fprintf(stderr, "%d: %ld accès mémoire sur zone contigüe de %ld Ko\n", */
	/* 	PARAM, a / PRECISION, MEMORY_SIZE / 1024); */

	return ((end - start) / PARAM);
}

int main(void)
{
	char *mem = alloc();
	uint64_t t = detect(mem);

	printf("%d %lu\n", PARAM, t);
	return EXIT_SUCCESS;
}
