#include "hwdetect.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef PARAM
#define PARAM 0
#endif

#define WARMUP 10000
#define PRECISION 2000000

#define CACHELINE_SIZE 64
#define CACHE_SIZE 32768
#define NB_WAYS 8
#define NB_SETS (CACHE_SIZE / NB_WAYS / CACHELINE_SIZE)
#define SKIP_SIZE (NB_SETS * CACHELINE_SIZE)

static inline char *alloc(size_t n) {
  size_t i;
  char *ret =
      mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ret == MAP_FAILED)
    abort();

  for (i = 0; i < n; i += PAGE_SIZE)
    ret[i] = 0;

  return ret;
}

struct thread_info {
  pthread_t thread_id;
  char *mem;
  uint8_t core_id;
  uint64_t time;
};

static inline void *detect(void *arg) {
  size_t i, assoc, p;
  uint64_t start, end;

  struct thread_info *tinfo = (struct thread_info *)arg;
  char *mem = tinfo->mem;
  setcore(tinfo->core_id);

  start = now();

  for (p = 0; p < PRECISION; p++)
    for (assoc = 0; assoc < NB_WAYS; assoc++)
      for (i = 0; i < NB_SETS; i++)
        writemem(mem + (i * CACHELINE_SIZE) + (assoc * SKIP_SIZE));

  end = now();
  tinfo->time = (end - start);

  return 0;
}

int main(void) {
  char *mem = alloc(NB_WAYS * SKIP_SIZE * 2);
  struct thread_info tinfo1 = (struct thread_info){.mem = mem, .core_id = 0};
  struct thread_info tinfo2 = (struct thread_info){
      .mem = mem + NB_WAYS * SKIP_SIZE * sizeof(char),
      .core_id = PARAM,
  };

  size_t i, assoc, p;
  for (p = 0; p < WARMUP; p++)
    for (assoc = 0; assoc < NB_WAYS; assoc++)
      for (i = 0; i < NB_SETS * 2; i++)
        writemem(mem + (i * CACHELINE_SIZE) + (assoc * SKIP_SIZE));

  pthread_create(&tinfo1.thread_id, NULL, &detect, (void *)&tinfo1);
  pthread_create(&tinfo2.thread_id, NULL, &detect, (void *)&tinfo2);

  pthread_join(tinfo1.thread_id, NULL);
  pthread_join(tinfo2.thread_id, NULL);

  printf("%d %lu\n", PARAM, tinfo1.time);
  return EXIT_SUCCESS;
}
