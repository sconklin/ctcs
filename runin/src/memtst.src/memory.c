#include <stdio.h>
#include "memory.h"

#if defined(__BSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
// BSD Version
void meminfo( struct memory *m ) {

  int mib[2], value;
  size_t len;
  mib[0] = CTL_HW;
  mib[1] = HW_PHYSMEM;
  len = sizeof(m->total_mem);
  sysctl(mib, 2, &m->total_mem, &len, NULL, 0);
  printf("Total mem: %d", m->total_mem);
#if 0
  m->used_mem =
  m->free_mem = 
  m->shared_mem = 
  m->buffer_mem =
  m->cached_mem =
  m->total_swap = 
  m->used_swap =  
  m->free_swap = 
#endif
}
#else

#define LINESIZE 1024
// Linux Version
void meminfo( struct memory *m ) {
  FILE *f;
  char line1[LINESIZE], line2[LINESIZE], line3[LINESIZE];

  f = fopen("/proc/meminfo", "r");

  fgets(line1, LINESIZE-1, f);
  fgets(line2, LINESIZE-1, f);
  fgets(line3, LINESIZE-1, f);

  sscanf(line2, "%s %lu %lu %lu %lu %lu %lu", line1,
	 &(m->total_mem), 
	 &(m->used_mem), 
	 &(m->free_mem), 
	 &(m->shared_mem), 
	 &(m->buffer_mem), 
	 &(m->cached_mem));

  sscanf(line3, "%s %lu %lu %lu", line1,
	 &(m->total_swap), 
	 &(m->used_swap), 
	 &(m->free_swap));
}

