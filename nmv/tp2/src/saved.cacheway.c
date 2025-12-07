#include "hwdetect.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef PARAM
#  define PARAM            1
#endif

#define MEMORY_SIZE        (1ul << 20)
#define CACHELINE_SIZE     64
#define CACHESIZE        32768
#define ASSOCIATIVITY      PARAM
#define NB_CACHE_ENRTIES   (CACHESIZE / CACHELINE_SIZE)
#define NB_ENTRIES_PER_WAY (NB_CACHE_ENRTIES / ASSOCIATIVITY)
#define SKIP_SIZE          (CACHELINE_SIZE * NB_ENTRIES_PER_WAY)
	

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

	for (p = 0; p < WARMUP; p++) {
		
		/* Pour toutes les ways */
		for (i = 0; i < ASSOCIATIVITY; i++) {
			/* On accede a toutes les entrees de la way */
			for (int j = 0; j < NB_ENTRIES_PER_WAY; j++) {
				writemem(mem + (i * CACHESIZE) + (j * CACHELINE_SIZE));
			}
		}
	}
	

	start = now();

	for (p = 0; p < PRECISION; p++)
		for (i = 0; i < ASSOCIATIVITY; i++) {
			/* On accede a toutes les entrees de la way */
			for (int j = 0; j < NB_ENTRIES_PER_WAY; j++) {
				writemem(mem + (i * SKIP_SIZE) + (j * CACHELINE_SIZE));
			}
		}

	end = now();

	return (end - start) / PRECISION;
}

int main(void)
{
	char *mem = alloc();
	uint64_t t = detect(mem);

	printf("%d %lu\n", PARAM, t);
	return EXIT_SUCCESS;
}
