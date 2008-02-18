#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*We only use total_mem, and total_swap.  We don't actually check that the rest are defined!!*/
/*In the event anything doesn't look like a number we set it to 1 (if we care) or 0 (if we do not*/
typedef struct {
       unsigned long total_mem;
        unsigned long used_mem;
        unsigned long free_mem;
        unsigned long shared_mem;
        unsigned long buffer_mem;
        unsigned long cached_mem;

        unsigned long total_swap;
        unsigned long used_swap;
        unsigned long free_swap;
}memory_from_free;

int verbose;

#if defined(__BSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
// BSD Version

memory_from_free meminfo(void) {
  memory_from_free m;
  memset(&m,0, sizeof(m));
  int mib[2], value;
  size_t len;
  mib[0] = CTL_HW;
  mib[1] = HW_PHYSMEM;
  len = sizeof(m.total_mem);
  sysctl(mib, 2, &m.total_mem, &len, NULL, 0);

  mib[1] = HW_PHYSMEM;
  len = sizeof(m.total_mem);
  sysctl(mib, 2, &m.total_mem, &len, NULL, 0);


  if(verbose) {
    printf("total_mem %ld\n", m.total_mem);
    printf("used_mem %ld\n", m.used_mem);
    printf("free_mem %ld\n", m.free_mem);
    printf("shared_mem %ld\n", m.shared_mem);
    printf("buffer_mem %ld\n", m.buffer_mem);
    printf("cached_mem %ld\n", m.cached_mem);
    printf("total_swap %ld\n", m.total_swap);
    printf("used_swap %ld\n", m.used_swap );
    printf("free_swap %ld\n", m.free_swap);
  }
  return (m);
}

#else

memory_from_free meminfo(void) {
  FILE* freehandle = popen("free", "r");
  char stuff[255];
  fgets(stuff, sizeof(stuff), freehandle);
  fgets(stuff, sizeof(stuff), freehandle);
  if (0 == strncmp(stuff, "Mem:", strlen("Mem:"))) {
  }
  if(verbose) {
    printf("\nFound (via free) memory: %s", stuff);
  } 
  char* stuffmem;
  strtok(stuff, " ");
  memory_from_free m;
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.total_mem = atol(stuffmem);
  }else {
     printf("\nWe didn't define total_swap!! Is free not in your path? Or does it use a non linux format? ");
     m.total_mem = 1;
  }
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.used_mem = atol(stuffmem);
  }else { m.used_mem = 0;}
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.free_mem = atol(stuffmem);
  }else { m.free_mem= 0;}
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.shared_mem = atol(stuffmem);
  }else { m.shared_mem= 0;}
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.buffer_mem = atol(stuffmem);
  }else { m.buffer_mem= 0;}
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.cached_mem = atol(stuffmem);
  }else { m.cached_mem= 0;}

 fgets(stuff, sizeof(stuff), freehandle);
  fgets(stuff, sizeof(stuff), freehandle);
  if (0 == strncmp(stuff, "Swap:", strlen("Swap:"))) {
  }
  if(verbose) {
    printf("\nFound (via free) swap: %s", stuff);
  }
  strtok(stuff, " ");

  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.total_swap = atol(stuffmem);
  }else {
     printf("\nWe didn't define total_swap!! Is free not in your path? Or does it use a non linux format? ");
     m.total_swap = 1;
  }
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.used_swap = atol(stuffmem);
  }else { m.used_swap= 0;}
  stuffmem = strtok(NULL, " ");
  if (isdigit(stuffmem[0])) {
    m.free_swap = atol(stuffmem);
  }else { m.used_swap= 0;}
  pclose(freehandle);

  if(verbose) {
    printf("total_mem %ld\n", m.total_mem);
    printf("used_mem %ld\n", m.used_mem);
    printf("free_mem %ld\n", m.free_mem);
    printf("shared_mem %ld\n", m.shared_mem);
    printf("buffer_mem %ld\n", m.buffer_mem);
    printf("cached_mem %ld\n", m.cached_mem);
    printf("total_swap %ld\n", m.total_swap);
    printf("used_swap %ld\n", m.used_swap );
    printf("free_swap %ld\n", m.free_swap);
  }
  return (m);
}
#endif
