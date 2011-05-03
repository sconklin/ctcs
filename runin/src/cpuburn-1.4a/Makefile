#
# Makefile for the cpuburn program, by Jason T. Collins
# .S from Robert Redelmeier's makefile added for 1.4
#
all: burnP6 burnMMX burnK6 burnP5 burnBX burnK7; 
dist-clean:
	rm -f ../../bin/burnP6
	rm -f ../../bin/burnMMX
	rm -f ../../bin/burnK6
	rm -f ../../bin/burnP5
	rm -f ../../bin/burnBX
	rm -f ../../bin/burnK7

install: all ;
	cp burnP6 ../../bin
	cp burnMMX ../../bin
	cp burnK6 ../../bin
	cp burnP5 ../../bin
	cp burnBX ../../bin
	cp burnK7 ../../bin

clean: ;
	rm -f *.o burnK6 burnP5 burnMMX burnP6 burnBX burnK7

.S:
	gcc -m32 -s -nostdlib -o $@ $<
