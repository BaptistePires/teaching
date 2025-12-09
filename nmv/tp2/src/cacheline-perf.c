#include "hwdetect.h"
#include "perf_events.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#ifndef PARAM
#  define PARAM            64
#endif

#define CACHELINE_SIZE     PARAM
#define MEMORY_SIZE        (32 * PAGE_SIZE)
#define WARMUP             10000
#define PRECISION          10000


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



static inline double detect(char *mem)
{
	size_t i, p;
	long long l1d_misses = 0;
	
	for (p = 0; p < WARMUP; p++)
		for (i = 0; i < MEMORY_SIZE; i += CACHELINE_SIZE)
			writemem(mem + i);


	int fd_l1_miss = open_cache_event(PERF_COUNT_HW_CACHE_L1D,
	PERF_COUNT_HW_CACHE_OP_READ,
	PERF_COUNT_HW_CACHE_RESULT_MISS);

	reset_perf_event(fd_l1_miss);

	for (p = 0; p < PRECISION; p++)
		for (i = 0; i < MEMORY_SIZE; i += CACHELINE_SIZE)
			writemem(mem + i);


	ioctl(fd_l1_miss, PERF_EVENT_IOC_DISABLE, 0);

	read(fd_l1_miss, &l1d_misses, sizeof(long long));
	close(fd_l1_miss);


	double miss_rate = (double)l1d_misses / (PRECISION * ((double)MEMORY_SIZE / CACHELINE_SIZE));
	return 1 - miss_rate;
}

int main(void)
{
	char *mem = alloc(MEMORY_SIZE);
	double t = detect(mem);

	printf("%d %f\n", PARAM, 100 * t);
	return EXIT_SUCCESS;
}
