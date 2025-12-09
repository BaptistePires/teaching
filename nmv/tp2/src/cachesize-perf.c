#include "hwdetect.h"
#include "perf_events.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef PARAM
#  define PARAM            0
#endif

#define MEMORY_SIZE        (1ul << 21)
#define WARMUP             10000
#define PRECISION          1000000

#define CACHELINE_SIZE     64
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

static inline double detect(char *mem)
{

	size_t i, p;
	long long l1d_misses = 0;

	for (p = 0; p < WARMUP; p++)
		for (i = 0; i < PARAM; i += CACHELINE_SIZE)
			writemem(mem + i);


	int fd_l1_miss = open_cache_event(PERF_COUNT_HW_CACHE_L1D,
	PERF_COUNT_HW_CACHE_OP_READ,
	PERF_COUNT_HW_CACHE_RESULT_MISS);

	reset_perf_event(fd_l1_miss);
	for (p = 0; p < PRECISION; p++)
		for (i = 0; i < PARAM; i += CACHELINE_SIZE)
			writemem(mem + i);

	ioctl(fd_l1_miss, PERF_EVENT_IOC_DISABLE, 0);

	read(fd_l1_miss, &l1d_misses, sizeof(long long));
	close(fd_l1_miss);


	double miss_rate = (double)l1d_misses / (PRECISION * ((double)PARAM / CACHELINE_SIZE));
	return 1 - miss_rate;
}

int main(void)
{
	char *mem = alloc();
	double t = detect(mem);

	printf("%d %f\n", PARAM, t);
	return EXIT_SUCCESS;
}
