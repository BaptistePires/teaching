#ifndef PERF_EVENTS_H
#define PERF_EVENTS_H

#include <linux/perf_event.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>




static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
}
static int open_cache_event(unsigned type, unsigned op, unsigned result)
{
	struct perf_event_attr pe;
	memset(&pe, 0, sizeof(struct perf_event_attr));
	pe.type = PERF_TYPE_HW_CACHE;
	pe.size = sizeof(struct perf_event_attr);
	pe.config = type | (op << 8) | (result << 16);
	pe.disabled = 1;
	pe.exclude_kernel = 1;
	pe.exclude_hv = 1;

	int fd = perf_event_open(&pe, 0, -1 , -1, 0);

	if (fd == -1) {
		perror("perf_event_open");
		exit(EXIT_FAILURE);
	}
	return fd;
}

static void reset_perf_event(int fd)
{
    	ioctl(fd, PERF_EVENT_IOC_RESET, 0);
	ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}


#endif