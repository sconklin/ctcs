
CFLAGS = -g -O3
CC = gcc
OBJS = main.o netburn.o
PROGS =	netburn

all:	${PROGS}

netburn:	${OBJS}
		${CC} ${CFLAGS} -o $@ ${OBJS}

clean:
		rm -f ${PROGS} ${OBJS}
