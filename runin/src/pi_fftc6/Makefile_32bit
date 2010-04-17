# ---- for GNU gcc ----
CC = gcc
OFLAGS_FFT = -O6 -ffast-math 
OFLAGS_PI = -O6 -ffast-math -mtune=pentium4 -mfpmath=sse,387 -msse3

# ---- for SUN WS cc ----
#CC = cc
#OFLAGS_FFT = -fast -xO5
#OFLAGS_PI = -fast -xO5

# ---- for DEC cc ----
#CC = cc
#OFLAGS_FFT = -fast -O6
#OFLAGS_PI = -fast -O6


# ---- use POSIX Thread ----
#CFLAGS_TH = -DUSE_CDFT_PTHREADS
#LFLAGS_TH = -lpthread

# ---- use 64-bit size FFT ----
#CFLAGS_FI = -DUSE_FFT_LONG_INT


CFLAGS = $(CFLAGS_TH) $(CFLAGS_FI)
LFLAGS = -lm $(LFLAGS_TH)


all: pi_ca pi_cs pi_cw dgt_div


pi_ca : pi_fftca.o fftsgx.o
	$(CC) pi_fftca.o fftsgx.o $(LFLAGS) -o $@

pi_cs : pi_fftcs.o fftsg_hx.o
	$(CC) pi_fftcs.o fftsg_hx.o $(LFLAGS) -o $@

pi_cw : pi_fftcw.o fftsg_hx.o
	$(CC) pi_fftcw.o fftsg_hx.o $(LFLAGS) -o $@

dgt_div : dgt_div.o
	$(CC) dgt_div.o -o $@


pi_fftca.o : pi_fftca.c
	$(CC) $(CFLAGS) $(OFLAGS_PI) -c $*.c -o $@

pi_fftcs.o : pi_fftcs.c
	$(CC) $(CFLAGS) $(OFLAGS_PI) -c $*.c -o $@

pi_fftcw.o : pi_fftcw.c
	$(CC) $(CFLAGS) $(OFLAGS_PI) -c $*.c -o $@


fftsgx.o : fftsgx.c
	$(CC) $(CFLAGS) $(OFLAGS_FFT) -c $*.c -o $@

fftsg_hx.o : fftsg_hx.c
	$(CC) $(CFLAGS) $(OFLAGS_FFT) -c $*.c -o $@


dgt_div.o : dgt_div.c
	$(CC) $(CFLAGS) -O -c $*.c -o $@


clean:
	rm -f *.o

