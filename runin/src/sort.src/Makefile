
COPTIMIZE = -O3 --combine-fwhole-prgram -ffunction-sections -static -msse2
CFLAGS2 = $(CFLAGS) -g ${COPTIMIZE}
CC = @gcc
OBJS = main.o error.o heap.o quick.o merge.o shell.o
PROGS =	sort

all:	${PROGS}

sort:	${OBJS}
		${CC} ${CFLAGS2} -o $@ ${OBJS}
install:
		@cp sort ../../bin/

clean:
		@rm -f ${PROGS} ${OBJS}
