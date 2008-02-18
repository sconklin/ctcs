PI(= 3.1415926535897932384626...) Calculation Program


FFT ver. 20011026, AGM ver. LG1.1.2-MP1.5.2af


Files:
    Makefile       : use 32bit int: digits <= 6000000000
    Makefile_64bit : use 64bit int
    Makefile_quad  : use long double FFT
    config.h       : machine depending macros
    dgt_div.c      : converter from pi.dat to SUPER_PI format
    fftsg_hx.c     : FFT Package - split-radix  - use no work areas
    fftsgx.c       : FFT Package - split-radix  - use work areas
    mathxl.c       : long double math-lib
    pi_fftca.c     : PI Calculation Program I   - standard version
                     -- use rdft() in "fft*gx.c"
    pi_fftcs.c     : PI Calculation Program II  - memory save version
                     -- use rdft() in "fft*g_hx.c"
    pi_fftcw.c     : PI Calculation Program III - memory swap version
                     -- use rdft() in "fft*g_hx.c"
    win32bin/
        Makefile     : - for intel C++ compiler


To Compile:
    Check macros in "config.h" and "Makefile*".
    Type "make clean; make -f [makefile]".


Execution files:
    pi_ca  : PI Calculation Program I   - double precision FFT
    pi_cs  : PI Calculation Program II  - double precision FFT
    pi_cw  : PI Calculation Program III - double precision FFT
    pi_caq : PI Calculation Program I   - long double precision FFT
    pi_csq : PI Calculation Program II  - long double precision FFT
    pi_cwq : PI Calculation Program III - long double precision FFT


Relationship between Number of Digits and FFT Length:
    ndigit = nfft*log_10(R), 
    R is a radix of multiple-precision format.
    R depends on DBL_ERROR_MARGIN and 
    FFT+machine+compiler's tolerance.
    log_10(R) >= 4 or 3 for double precision FFT.
    log_10(R) >= 12 for long double precision FFT.


Number of Floating Point Operations:
    42*nfft*(log_2(nfft))^2           [Operations]


Memory Use (Bytes):
    pi_ca(32bit)        : 52*nfft
    pi_cs(32bit)        : 36*nfft
    pi_cw(32bit)        : 14*nfft (swap: 16*nfft)
    pi_ca(64bit)        : 76*nfft
    pi_cs(64bit)        : 36*nfft
    pi_cw(64bit)        : 14*nfft (swap: 16*nfft)
    pi_caq(long double) : 104*nfft
    pi_csq(long double) : 96*nfft
    pi_cwq(long double) : 40*nfft (swap: 48*nfft)


AGM Algorithm:
  ---- a formula based on the AGM (Arithmetic-Geometric Mean) ----
    c = sqrt(0.125);
    a = 1 + 3 * c;
    b = sqrt(a);
    e = b - 0.625;
    b = 2 * b;
    c = e - c;
    a = a + e;
    npow = 4;
    do {
        npow = 2 * npow;
        e = (a + b) / 2;
        b = sqrt(a * b);
        e = e - b;
        b = 2 * b;
        c = c - e;
        a = e + b;
    } while (e > SQRT_SQRT_EPSILON);
    e = e * e / 4;
    a = a + b;
    pi = (a * a - e - e / 2) / (a * c - e) / npow;
  ---- modification ----
    This is a modified version of Gauss-Legendre formula
    (by T.Ooura). It is faster than original version.


Reference:
    1. E.Salamin, 
       Computation of PI Using Arithmetic-Geometric Mean, 
       Mathematics of Computation, Vol.30 1976.
    2. R.P.Brent, 
       Fast Multiple-Precision Evaluation of Elementary Functions, 
       J. ACM 23 1976.
    3. D.Takahasi, Y.Kanada, 
       Calculation of PI to 51.5 Billion Decimal Digits on 
       Distributed Memoriy Parallel Processors, 
       Transactions of Information Processing Society of Japan, 
       Vol.39 No.7 1998.
    4. T.Ooura, 
       Improvement of the PI Calculation Algorithm and 
       Implementation of Fast Multiple-Precision Computation, 
       Information Processing Society of Japan SIG Notes, 
       98-HPC-74, 1998.


Copyright
    source files:
        Copyright(C) 1999,2001 Takuya OOURA
        Email: ooura@kurims.kyoto-u.ac.jp
        URL:   http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html
        You may use, copy, modify this code for any purpose and 
        without fee. You may distribute this ORIGINAL package.

