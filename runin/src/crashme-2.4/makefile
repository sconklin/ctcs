all: crashme pddet

crashme:	crashme.o
	cc -o crashme crashme.o
crashme.o: crashme.c
	cc -c crashme.c

pddet:	pddet.o
	cc -o pddet pddet.o
pddet.o: pddet.c
	cc -c pddet.c

install: all
	cp crashme ../../bin

dist-clean: clean
	rm -f ../../bin/crashme

clean:
	-rm -f crashme pddet *.o core crashme.txt crashme.zip \
            crashme.tgz

# create for dist for people without nroff

crashme.txt: crashme.1
	nroff -man crashme.1 | col -bx > crashme.txt


DIST_FILES = crashme.1 crashme.c crashme.html \
             crashme.opt descrip.mms make.bat makefile \
             makefile.wnt pddet.c crashme.txt

crashme.zip: $(DIST_FILES)
	zip -k -l -u crashme.zip $(DIST_FILES)

crashme.tgz: $(DIST_FILES)
	tar cvf - $(DIST_FILES) | gzip -v > crashme.tgz

check:
	nsgmls -s crashme.html

dist: crashme.zip crashme.tgz
