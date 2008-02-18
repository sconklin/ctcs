CC = icl
OFLAGS = -O3 -G7 -Oa -QaxiMKW -Qipo -Qpc80

CFLAGS_TH = -DUSE_CDFT_WINTHREADS

CFLAGS = -DPI_OUT_LOGFILE


all: pi_ca.exe pi_cs.exe pi_cw.exe pi_ca_thread.exe pi_cs_thread.exe pi_cw_thread.exe dgt_div.exe


pi_ca.exe : ../pi_fftca.c ../fftsgx.c
	$(CC) $(CFLAGS) $(OFLAGS) ../pi_fftca.c ../fftsgx.c -o $@

pi_cs.exe : ../pi_fftcs.c ../fftsg_hx.c
	$(CC) $(CFLAGS) $(OFLAGS) ../pi_fftcs.c ../fftsg_hx.c -o $@

pi_cw.exe : ../pi_fftcw.c ../fftsg_hx.c
	$(CC) $(CFLAGS) $(OFLAGS) ../pi_fftcw.c ../fftsg_hx.c -o $@


pi_ca_thread.exe : ../pi_fftca.c ../fftsgx.c
	$(CC) $(CFLAGS) $(CFLAGS_TH) $(OFLAGS) ../pi_fftca.c ../fftsgx.c -o $@

pi_cs_thread.exe : ../pi_fftcs.c ../fftsg_hx.c
	$(CC) $(CFLAGS) $(CFLAGS_TH) $(OFLAGS) ../pi_fftcs.c ../fftsg_hx.c -o $@

pi_cw_thread.exe : ../pi_fftcw.c ../fftsg_hx.c
	$(CC) $(CFLAGS) $(CFLAGS_TH) $(OFLAGS) ../pi_fftcw.c ../fftsg_hx.c -o $@


dgt_div.exe : ../dgt_div.c
	$(CC) -O2 ../dgt_div.c -o $@


clean:
	del *.obj

