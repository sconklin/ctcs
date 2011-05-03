#
# Makefile for source packages inside Cerberus.
#

include ../../build.mk

all: io_zone prandom chartst memtester memtst sort crashmex random pi flushb linux_only_stuff
# Linux only packages
ifndef __BSD__
linux_only_stuff:	netburn
else
linux_only_stuff:;
endif

dist-clean: 
	rm -f ../bin/random
	rm -f ../bin/prandom
	# if flushb gets fixed, change this
	rm -f ../bin/flushb.real
	rm -f ../chartst
	rm -f ../memtst
	rm -f ../bin/smartctl
	rm -f ../bin/iozone
	rm -f ../bin/fileop
	rm -f ../bin/pi_c*
	rm -f /sort.src
	rm -f ./netburn
	sh -c "$(MAKE) -C cpuburn dist-clean"
	sh -c "$(MAKE) -C crashme dist-clean"
	sh -c "$(MAKE) -C memtester-4.2.1 clean"
install: all 
	cp random ../bin
	cp prandom ../bin
	cp flushb ../bin/flushb.real
	cp chartst ..
	cp memtst.src/memtst ../memtst
	cp memtester-4.2.1/memtester ../bin/memtester
	cp sort.src/sort ../bin
	cp iozone3_347/src/current/iozone ../bin
	cp iozone3_347/src/current/fileop ../bin
	cp pi_fftc6/pi_c* ../bin
	sh -c "$(MAKE) -C cpuburn install"
	sh -c "$(MAKE) -C crashme install >> /dev/null 2>&1"
ifndef __BSD__
	cp netburn.src/netburn ../bin
endif

memtester: 
	$(MAKE) -C memtester-4.2.1

memtst: 
	$(MAKE) -C memtst.src

sort:
	$(MAKE) -C sort.src

netburn:
	$(MAKE) -C netburn.src	

io_zone:
ifdef __BSD__
	$(MAKE) -C iozone3_347/src/current freebsd
else
	$(MAKE) -C iozone3_347/src/current linux
endif


ifdef __BSD__
flushb: flushbsd.o
	$(CC) $(CFLAGS) -o flushb flushbsd.o -lm
else
flushb: flushb.o 
	$(CC) $(CFLAGS) -o flushb flushb.o -lm
endif


random: random.o 
	$(CC) $(CFLAGS) -o random random.o -lm

prandom: prandom.o 
	$(CC) $(CFLAGS) -o prandom prandom.o -lm

chartst: chartst.o 
	$(CC) $(CFLAGS) -o chartst chartst.o -lm

crashmex: 
	$(MAKE) -C crashme >>/dev/null 2>&1

pi:
	$(MAKE) -C pi_fftc6
	
clean: ;
	rm -f *.o prandom chartst flushb random
	$(MAKE) -C memtst.src clean
	$(MAKE) -C memtester-4.2.1 clean
	$(MAKE) -C cpuburn clean
	$(MAKE) -C crashme clean
	$(MAKE) -C sort.src clean
	$(MAKE) -C netburn.src clean
	$(MAKE) -C iozone3_347/src/current clean
	$(MAKE) -C pi_fftc6 clean
