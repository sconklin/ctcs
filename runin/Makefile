#
# Makefile for the Cerberus Runin Authoring Program.
#
include ../build.mk

all: src-all symlinks;

symlinks:
	ln -sf messages-info allmessages-info
	ln -sf blockrdtst sblockrdtst
	ln -sf blockrdtst-info sblockrdtst-info
	ln -sf wblockrdtst swblockrdtst
	ln -sf wblockrdtst-info swblockrdtst-info
	ln -sf data sdata
	ln -sf data-info sdata-info
	ln -sf destructiveblocktst sdestructiveblocktst
	ln -sf destructiveblocktst-info sdestructiveblocktst-info
	ln -sf traverseread-info straverseread-info
	ln -sf traverseread straverseread
	ln -sf iozone-info siozone-info
	ln -sf iozone siozone

install: all src-install;

clean: src-clean;
	rm -f sblockrdtst sblockrdtst-info sdata sdata-info sdestructiveblocktst straverseread straverseread-info sdestructiveblocktst-info allmessages-info ltptemp.*
	rm -f tmpdac960.* tmpdac960_2.*

src-all:
	$(MAKE) -C src all

src-install:
	$(MAKE) -C src install

src-clean:
	$(MAKE) -C src clean

src-dist-clean: src-clean 
	$(MAKE) -C src dist-clean

dist-clean: clean src-dist-clean ;
