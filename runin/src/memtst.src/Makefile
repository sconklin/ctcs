#
#
# define -D__BSD__ for bsd release
#
include ../../../build.mk

ifdef __BSD__
	MEMTST_CFLAGS=$(CFLAGS) -fno-unroll-loops -fno-unroll-all-loops -D__BSD__
else
	MEMTST_CFLAGS=$(CFLAGS) -fno-unroll-loops -fno-unroll-all-loops
endif

all: sizeofint memtst maxalloc;


sizeofint: sizeofint.c;
	$(CC) sizeofint.c -o sizeofint

sizeofint.h: sizeofint ;
	sh -c "./sizeofint > sizeofint.h"

maxalloc: sizeofint.h ;
	$(CC) $(MEMTST_CFLAGS) -Wall maxalloc.c -o maxalloc

memtst: memory.h sizeofint.h memtst.h memtst.c memtst_main.c;
	$(CC) $(MEMTST_CFLAGS) -c memtst.c -o memtst.o
	$(CC) $(MEMTST_CFLAGS)  memtst_main.c memtst.o -o memtst

clean:
	rm -f *.o memtst maxalloc sizeofint.h sizeofint

