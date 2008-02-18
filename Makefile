#
# Makefile for the Cerberus Runin Authoring Program.
#

# Debug
#CC=cc
#CFLAGS=-Wall -g

#include HostVars.mk
include build.mk

export CC=gcc
export CFLAGS= -O6 -mtune=i686 -finline-functions -funroll-loops

all: symlinks;
	$(MAKE) -C runin all
	$(MAKE) -C runin install
	mkdir -p log

HostVars.mk:
	sh -c "echo # this file is dynamically generated > HostVars.mk"
	sh -c "echo export MACHINE=`uname -m` >> HostVars.mk"

symlinks:
	$(MAKE) -C runin symlinks 

spec: ctcs.spec ;


ctcs.spec:
	./genspec

requirements: ;
	./check-requirements

test: symlinks ;
	./check-syntax
	$(MAKE) dist-clean

install: all ;

clean: ;
	$(MAKE) -C runin clean
	rm -f `find ./* -name "core" -print`
	rm -f HostVars.mk
	
src-dist-clean: ;
	$(MAKE) -C runin dist-clean

dist-clean: src-dist-clean clean ;
	rm -rf .*.tcf.log.*  .newburn.old.* newburn.tcf newburn2.tcf newburn.params newburn2.params
	rm -f `find -name "*~" -print`
	rm -f `find -name "#*#" -print`
cvs-clean:
	rm -rf `find . -name CVS`
	$(MAKE) dist-clean
