ifndef MAKE
    ifeq (`uname -s`, FreeBSD)
    	MAKE= gmake
	export __BSD__ = 1
    else
    	MAKE= make
    endif
endif
