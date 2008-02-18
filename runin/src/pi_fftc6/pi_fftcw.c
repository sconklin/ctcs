/*
---- calculation of PI(= 3.14159...) using FFT and AGM ----
    by T.Ooura

Example compilation:
    GNU      : gcc -O6 -ffast-math pi_fftcw.c fftsg_hx.c -lm -o pi_cws
    SUN      : cc -fast -xO5 pi_fftcw.c fftsg_hx.c -lm -o pi_cws
    Microsoft: cl /O2 /G6 pi_fftcw.c fftsg_hx.c /Fepi_csw.exe
    ...
    etc.
*/

#include "config.h"
#define PI_FFTC_VER "ver. LG1.1.2-MP1.5.2af.memswap"


void f_swap_in(fft_int bytesize, char *f_name, char *in);
void f_swap_out(fft_int bytesize, char *f_name, char *out);
void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
fft_int mp_cmp(fft_int n, fft_int radix, fft_int in1[], fft_int in2[]);
void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
void mp_imul(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
fft_int mp_idiv(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
fft_float mp_mul_radix_test(fft_int n, fft_int radix, fft_int nfft, 
        fft_float tmpfft[]);
void mp_mul(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
        fft_int nfft, fft_float tmpfft[]);
void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, fft_float outfft[]);
fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmpfft[]);
void mp_sprintf(fft_int n, fft_int log10_radix, fft_int in[], char out[]);
void mp_sscanf(fft_int n, fft_int log10_radix, char in[], fft_int out[]);
void mp_fprintf(fft_int n, fft_int log10_radix, fft_int in[], FILE *fout);
fft_int mp_chksum(fft_int n, fft_int in[]);


int main()
{
    fft_int nfft, log2_nfft, radix, log10_radix, n, npow, nprc, sum;
    fft_float err;
    fft_int *a, *b, *c;
    fft_float *d1;
    char *f_name_tmpa = SWAP_FILE_TMPA;
    char *f_name_tmpc = SWAP_FILE_TMPC;
    double x, memsize;
    time_t t_1, t_2;
    FILE *f_out;
#ifdef PI_OUT_LOGFILE
    FILE *f_log;
    f_log = fopen("pi.log", "w");
#endif
    
    printf("Calculation of PI using FFT and AGM, %s\n", PI_FFTC_VER);
#ifdef PI_OUT_LOGFILE
    fprintf(f_log, "Calculation of PI using FFT and AGM, %s\n", PI_FFTC_VER);
#endif
    printf("length of FFT =?\n");
    scanf("%lg", &x);
    nfft = (fft_int) x;
    
    printf("initializing...\n");
    for (log2_nfft = 1; (1 << log2_nfft) < nfft; log2_nfft++);
    nfft = 1 << log2_nfft;
    n = nfft + 2;
    a = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    b = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    c = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    memsize = 3 * (2 * sizeof(fft_int) + n * sizeof(dgt_int));
    d1 = (fft_float *) malloc((nfft + 2) * sizeof(fft_float));
    memsize += (nfft + 2) * sizeof(fft_float);
    if (d1 == NULL) {
        printf("Allocation Failure!\n");
        exit(1);
    }
    /* ---- radix test ---- */
    log10_radix = 1;
    radix = 10;
    err = mp_mul_radix_test(n, radix, nfft, d1);
    err += FFT_FLOAT_EPSILON * (n * radix * radix / 4);
    while (100 * err < FFT_ERROR_MARGIN && radix <= DGT_INT_MAX / 20) {
        err *= 100;
        log10_radix++;
        radix *= 10;
    }
    printf("nfft= %.0f\nradix= %.0f\nerror_margin= %g\nmem_alloc_size= %.0f\n", (double) nfft, (double) radix, (double) err, memsize);
    printf("calculating %.0f digits of PI...\n", (double) (log10_radix * (n - 2)));
#ifdef PI_OUT_LOGFILE
    fprintf(f_log, "nfft= %.0f\nradix= %.0f\nerror_margin= %g\nmem_alloc_size= %.0f\n", (double) nfft, (double) radix, (double) err, memsize);
    fprintf(f_log, "calculating %.0f digits of PI...\n", (double) (log10_radix * (n - 2)));
#endif
    /* ---- time check ---- */
    time(&t_1);
    /*
     * ---- a formula based on the AGM (Arithmetic-Geometric Mean) ----
     *   c = sqrt(0.125);
     *   a = 1 + 3 * c;
     *   b = sqrt(a);
     *   e = b - 0.625;
     *   b = 2 * b;
     *   c = e - c;
     *   a = a + e;
     *   npow = 4;
     *   do {
     *       npow = 2 * npow;
     *       e = (a + b) / 2;
     *       b = sqrt(a * b);
     *       e = e - b;
     *       b = 2 * b;
     *       c = c - e;
     *       a = e + b;
     *   } while (e > SQRT_SQRT_EPSILON);
     *   e = e * e / 4;
     *   a = a + b;
     *   pi = (a * a - e - e / 2) / (a * c - e) / npow;
     * ---- modification ----
     *   This is a modified version of Gauss-Legendre formula
     *   (by T.Ooura). It is faster than original version.
     * ---- reference ----
     *   1. E.Salamin, 
     *      Computation of PI Using Arithmetic-Geometric Mean, 
     *      Mathematics of Computation, Vol.30 1976.
     *   2. R.P.Brent, 
     *      Fast Multiple-Precision Evaluation of Elementary Functions, 
     *      J. ACM 23 1976.
     *   3. D.Takahasi, Y.Kanada, 
     *      Calculation of PI to 51.5 Billion Decimal Digits on 
     *      Distributed Memoriy Parallel Processors, 
     *      Transactions of Information Processing Society of Japan, 
     *      Vol.39 No.7 1998.
     *   4. T.Ooura, 
     *      Improvement of the PI Calculation Algorithm and 
     *      Implementation of Fast Multiple-Precision Computation, 
     *      Information Processing Society of Japan SIG Notes, 
     *      98-HPC-74, 1998.
     */
    /* ---- tmpc = c = 1 / sqrt(8) ---- */
    mp_invisqrt(n, radix, 8, c, a, b, nfft, d1);
    f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpc, (char *) c);
    /* ---- b = sqrt(1 + 3 * c) ---- */
    mp_imul(n, radix, c, 3, a);
    mp_sscanf(n, log10_radix, "1", b);
    mp_add(n, radix, a, b, a);
    mp_sqrt(n, radix, a, b, c, nfft, d1);
    /* ---- tmpc = b - 0.625 - (c = tmpc) ---- */
    mp_sscanf(n, log10_radix, "0.625", a);
    mp_sub(n, radix, b, a, a);
    f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpc, (char *) c);
    mp_sub(n, radix, a, c, a);
    f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpc, (char *) a);
    /* ---- a = 0.375 + 3 * c + b ---- */
    mp_imul(n, radix, c, 3, a);
    mp_sscanf(n, log10_radix, "0.375", c);
    mp_add(n, radix, a, c, a);
    mp_add(n, radix, a, b, a);
    /* ---- b = 2 * b ---- */
    mp_add(n, radix, b, b, b);
    /* ---- time check ---- */
    time(&t_2);
    sum = mp_chksum(n, c);
    printf("AGM iteration,\ttime= %.0f,\tchksum= %x\n", difftime(t_2, t_1), (int) sum);
#ifdef PI_OUT_LOGFILE
    fprintf(f_log, "AGM iteration,\ttime= %.0f,\tchksum= %x\n", difftime(t_2, t_1), (int) sum);
    fflush(f_log);
#endif
    npow = 4;
    do {
        npow *= 2;
        /* ---- tmpa = (a + b) / 2 ---- */
        mp_add(n, radix, a, b, c);
        mp_idiv_2(n, radix, c, c);
        f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
                f_name_tmpa, (char *) c);
        /* ---- b = sqrt(a * b) ---- */
        mp_mul(n, radix, a, b, a, c, nfft, d1);
        mp_sqrt(n, radix, a, b, c, nfft, d1);
        /* ---- c = tmpa - b ---- */
        f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
                f_name_tmpa, (char *) c);
        mp_sub(n, radix, c, b, c);
        /* ---- b = 2 * b ---- */
        mp_add(n, radix, b, b, b);
        /* ---- tmpc = tmpc - c ---- */
        f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
                f_name_tmpc, (char *) a);
        mp_sub(n, radix, a, c, a);
        f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
                f_name_tmpc, (char *) a);
        /* ---- a = c + b ---- */
        mp_add(n, radix, c, b, a);
        /* ---- convergence check ---- */
        nprc = -c[1];
        if (c[0] == 0) {
            nprc = n;
        }
        /* ---- time check ---- */
        time(&t_2);
        sum = mp_chksum(n, c);
        printf("precision= %.0f,\ttime= %.0f,\tchksum= %x\n", (double) (4 * nprc * log10_radix), difftime(t_2, t_1), (int) sum);
#ifdef PI_OUT_LOGFILE
        fprintf(f_log, "precision= %.0f,\ttime= %.0f,\tchksum= %x\n", (double) (4 * nprc * log10_radix), difftime(t_2, t_1), (int) sum);
        fflush(f_log);
#endif
    } while (4 * nprc <= n);
    /* ---- tmpa = c * c / 4 (half precision) ---- */
    mp_idiv_2(n, radix, c, c);
    mp_squh(n, radix, c, c, nfft, d1);
    f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpa, (char *) c);
    /* ---- a = a + b ---- */
    mp_add(n, radix, a, b, a);
    /* ---- a = a * a - tmpa - tmpa / 2, c = a * tmpc - tmpa ---- */
    f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpc, (char *) c);
    mp_mul(n, radix, c, a, c, b, nfft, d1);
    mp_squ(n, radix, a, a, b, nfft, d1);
    f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpa, (char *) b);
    mp_sub(n, radix, c, b, c);
    mp_sub(n, radix, a, b, a);
    mp_idiv_2(n, radix, b, b);
    mp_sub(n, radix, a, b, a);
    /* ---- a = a / c / npow ---- */
    f_swap_out(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpa, (char *) a);
    mp_inv(n, radix, c, b, a, nfft, d1);
    f_swap_in(2 * sizeof(fft_int) + n * sizeof(dgt_int), 
            f_name_tmpa, (char *) a);
    mp_mul(n, radix, a, b, a, c, nfft, d1);
    mp_idiv(n, radix, a, npow, a);
    remove(f_name_tmpa);
    remove(f_name_tmpc);
    /* ---- time check ---- */
    time(&t_2);
    sum = mp_chksum(n, a);
    /* ---- output ---- */
    f_out = fopen("pi.dat", "w");
    printf("writing pi.dat...\n");
    mp_fprintf(n - 1, log10_radix, a, f_out);
    fclose(f_out);
    free(d1);
    free(c);
    free(b);
    free(a);
    /* ---- difftime ---- */
    printf("Total %.0f sec. (real time),\tchksum= %x\n", difftime(t_2, t_1), (int) sum);
#ifdef PI_OUT_LOGFILE
    fprintf(f_log, "Total %.0f sec. (real time),\tchksum= %x\n", difftime(t_2, t_1), (int) sum);
    fclose(f_log);
#endif
    return 0;
}


/* -------- multiple precision routines -------- */


#include <math.h>
#include <float.h>
#include <stdio.h>

/* ---- floating point format ----
    data := data[0] * pow(radix, data[1]) * 
            (data[2] + data[3]/radix + data[4]/radix/radix + ...), 
    data[0]       : sign (1;data>0, -1;data<0, 0;data==0)
    data[1]       : exponent (0;data==0)
    data[2...n+1] : digits
   ---- function prototypes ----
    void f_swap_in(fft_int bytesize, char *f_name, char *in);
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    fft_int mp_cmp(fft_int n, fft_int radix, fft_int in1[], fft_int in2[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_imul(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
    fft_int mp_idiv(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
    void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    fft_float mp_mul_radix_test(fft_int n, fft_int radix, fft_int nfft, 
            fft_float tmpfft[]);
    void mp_mul(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
    void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
            fft_int nfft, fft_float tmpfft[]);
    fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
    fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
    fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmpfft[]);
    void mp_sprintf(fft_int n, fft_int log10_radix, fft_int in[], char out[]);
    void mp_sscanf(fft_int n, fft_int log10_radix, char in[], fft_int out[]);
    void mp_fprintf(fft_int n, fft_int log10_radix, fft_int in[], FILE *fout);
   ----
*/


/* -------- memory swap routines -------- */


void f_swap_in(fft_int bytesize, char *f_name, char *in)
{
    FILE *f_in;
    fft_int j;
    
    f_in = fopen(f_name, "rb");
    for (j = 0; j < bytesize; j++) {
        in[j] = (char) getc(f_in);
    }
    fclose(f_in);
}


void f_swap_out(fft_int bytesize, char *f_name, char *out)
{
    FILE *f_out;
    fft_int j;
    
    f_out = fopen(f_name, "wb");
    for (j = 0; j < bytesize; j++) {
        putc((fft_int) out[j], f_out);
    }
    fclose(f_out);
}


/* -------- mp_load routines -------- */


void mp_load_0(fft_int n, fft_int radix, fft_int out[])
{
    fft_int j;
    dgt_int *outdgt;
    
    outdgt = (dgt_int *) &out[2];
    out[0] = 0;
    out[1] = 0;
    for (j = 0; j < n; j++) {
        outdgt[j] = 0;
    }
}


void mp_load_1(fft_int n, fft_int radix, fft_int out[])
{
    fft_int j;
    dgt_int *outdgt;
    
    outdgt = (dgt_int *) &out[2];
    out[0] = 1;
    out[1] = 0;
    outdgt[0] = 1;
    for (j = 1; j < n; j++) {
        outdgt[j] = 0;
    }
}


void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[])
{
    fft_int j;
    dgt_int *indgt, *outdgt;
    
    indgt = (dgt_int *) &in[2];
    outdgt = (dgt_int *) &out[2];
    out[0] = in[0];
    out[1] = in[1];
    for (j = 0; j < n; j++) {
        outdgt[j] = indgt[j];
    }
}


void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[])
{
    fft_int j, x;
    dgt_int *inoutdgt;
    
    inoutdgt = (dgt_int *) &inout[2];
    if (m < n) {
        for (j = n - 1; j > m; j--) {
            inoutdgt[j] = 0;
        }
        x = 2 * inoutdgt[m];
        inoutdgt[m] = 0;
        if (x >= radix) {
            for (j = m - 1; j >= 0; j--) {
                x = inoutdgt[j] + 1;
                if (x < radix) {
                    inoutdgt[j] = (dgt_int) x;
                    break;
                }
                inoutdgt[j] = 0;
            }
            if (x >= radix) {
                inoutdgt[0] = 1;
                inout[1]++;
            }
        }
    }
}


/* -------- mp_add routines -------- */


fft_int mp_cmp(fft_int n, fft_int radix, fft_int in1[], fft_int in2[])
{
    fft_int mp_unsgn_cmp(fft_int n, fft_int in1[], fft_int in2[]);
    
    if (in1[0] > in2[0]) {
        return 1;
    } else if (in1[0] < in2[0]) {
        return -1;
    }
    return in1[0] * mp_unsgn_cmp(n, &in1[1], &in2[1]);
}


void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[])
{
    fft_int mp_unsgn_cmp(fft_int n, fft_int in1[], fft_int in2[]);
    fft_int mp_unexp_add(fft_int n, fft_int radix, fft_int expdif, 
            dgt_int in1[], dgt_int in2[], dgt_int out[]);
    fft_int mp_unexp_sub(fft_int n, fft_int radix, fft_int expdif, 
            dgt_int in1[], dgt_int in2[], dgt_int out[]);
    fft_int outsgn, outexp, expdif;
    
    expdif = in1[1] - in2[1];
    outexp = in1[1];
    if (expdif < 0) {
        outexp = in2[1];
    }
    outsgn = in1[0] * in2[0];
    if (outsgn >= 0) {
        if (outsgn > 0) {
            outsgn = in1[0];
        } else {
            outsgn = in1[0] + in2[0];
            outexp = in1[1] + in2[1];
            expdif = 0;
        }
        if (expdif >= 0) {
            outexp += mp_unexp_add(n, radix, expdif, 
                    (dgt_int *) &in1[2], (dgt_int *) &in2[2], 
                    (dgt_int *) &out[2]);
        } else {
            outexp += mp_unexp_add(n, radix, -expdif, 
                    (dgt_int *) &in2[2], (dgt_int *) &in1[2], 
                    (dgt_int *) &out[2]);
        }
    } else {
        outsgn = mp_unsgn_cmp(n, &in1[1], &in2[1]);
        if (outsgn >= 0) {
            expdif = mp_unexp_sub(n, radix, expdif, 
                    (dgt_int *) &in1[2], (dgt_int *) &in2[2], 
                    (dgt_int *) &out[2]);
        } else {
            expdif = mp_unexp_sub(n, radix, -expdif, 
                    (dgt_int *) &in2[2], (dgt_int *) &in1[2], 
                    (dgt_int *) &out[2]);
        }
        outexp -= expdif;
        outsgn *= in1[0];
        if (expdif == n) {
            outsgn = 0;
        }
    }
    if (outsgn == 0) {
        outexp = 0;
    }
    out[0] = outsgn;
    out[1] = outexp;
}


void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[])
{
    fft_int mp_unsgn_cmp(fft_int n, fft_int in1[], fft_int in2[]);
    fft_int mp_unexp_add(fft_int n, fft_int radix, fft_int expdif, 
            dgt_int in1[], dgt_int in2[], dgt_int out[]);
    fft_int mp_unexp_sub(fft_int n, fft_int radix, fft_int expdif, 
            dgt_int in1[], dgt_int in2[], dgt_int out[]);
    fft_int outsgn, outexp, expdif;
    
    expdif = in1[1] - in2[1];
    outexp = in1[1];
    if (expdif < 0) {
        outexp = in2[1];
    }
    outsgn = in1[0] * in2[0];
    if (outsgn <= 0) {
        if (outsgn < 0) {
            outsgn = in1[0];
        } else {
            outsgn = in1[0] - in2[0];
            outexp = in1[1] + in2[1];
            expdif = 0;
        }
        if (expdif >= 0) {
            outexp += mp_unexp_add(n, radix, expdif, 
                    (dgt_int *) &in1[2], (dgt_int *) &in2[2], 
                    (dgt_int *) &out[2]);
        } else {
            outexp += mp_unexp_add(n, radix, -expdif, 
                    (dgt_int *) &in2[2], (dgt_int *) &in1[2], 
                    (dgt_int *) &out[2]);
        }
    } else {
        outsgn = mp_unsgn_cmp(n, &in1[1], &in2[1]);
        if (outsgn >= 0) {
            expdif = mp_unexp_sub(n, radix, expdif, 
                    (dgt_int *) &in1[2], (dgt_int *) &in2[2], 
                    (dgt_int *) &out[2]);
        } else {
            expdif = mp_unexp_sub(n, radix, -expdif, 
                    (dgt_int *) &in2[2], (dgt_int *) &in1[2], 
                    (dgt_int *) &out[2]);
        }
        outexp -= expdif;
        outsgn *= in1[0];
        if (expdif == n) {
            outsgn = 0;
        }
    }
    if (outsgn == 0) {
        outexp = 0;
    }
    out[0] = outsgn;
    out[1] = outexp;
}


/* -------- mp_add child routines -------- */


fft_int mp_unsgn_cmp(fft_int n, fft_int in1[], fft_int in2[])
{
    fft_int j, cmp;
    dgt_int *in1dgt, *in2dgt;
    
    in1dgt = (dgt_int *) &in1[1];
    in2dgt = (dgt_int *) &in2[1];
    cmp = in1[0] - in2[0];
    for (j = 0; j < n && cmp == 0; j++) {
        cmp = in1dgt[j] - in2dgt[j];
    }
    if (cmp > 0) {
        cmp = 1;
    } else if (cmp < 0) {
        cmp = -1;
    }
    return cmp;
}


fft_int mp_unexp_add(fft_int n, fft_int radix, fft_int expdif, 
        dgt_int in1[], dgt_int in2[], dgt_int out[])
{
    fft_int j, x, carry;
    
    carry = 0;
    if (expdif == 0 && in1[0] + in2[0] >= radix) {
        x = in1[n - 1] + in2[n - 1];
        carry = x >= radix ? -1 : 0;
        for (j = n - 1; j > 0; j--) {
            x = in1[j - 1] + in2[j - 1] - carry;
            carry = x >= radix ? -1 : 0;
            out[j] = (dgt_int) (x - (radix & carry));
        }
        out[0] = (dgt_int) -carry;
    } else {
        if (expdif > n) {
            expdif = n;
        }
        for (j = n - 1; j >= expdif; j--) {
            x = in1[j] + in2[j - expdif] - carry;
            carry = x >= radix ? -1 : 0;
            out[j] = (dgt_int) (x - (radix & carry));
        }
        for (j = expdif - 1; j >= 0; j--) {
            x = in1[j] - carry;
            carry = x >= radix ? -1 : 0;
            out[j] = (dgt_int) (x - (radix & carry));
        }
        if (carry != 0) {
            for (j = n - 1; j > 0; j--) {
                out[j] = out[j - 1];
            }
            out[0] = (dgt_int) -carry;
        }
    }
    return -carry;
}


fft_int mp_unexp_sub(fft_int n, fft_int radix, fft_int expdif, 
        dgt_int in1[], dgt_int in2[], dgt_int out[])
{
    fft_int j, x, borrow, ncancel;
    
    if (expdif > n) {
        expdif = n;
    }
    borrow = 0;
    for (j = n - 1; j >= expdif; j--) {
        x = in1[j] - in2[j - expdif] + borrow;
        borrow = x < 0 ? -1 : 0;
        out[j] = (dgt_int) (x + (radix & borrow));
    }
    for (j = expdif - 1; j >= 0; j--) {
        x = in1[j] + borrow;
        borrow = x < 0 ? -1 : 0;
        out[j] = (dgt_int) (x + (radix & borrow));
    }
    ncancel = 0;
    for (j = 0; j < n && out[j] == 0; j++) {
        ncancel = j + 1;
    }
    if (ncancel > 0 && ncancel < n) {
        for (j = 0; j < n - ncancel; j++) {
            out[j] = out[j + ncancel];
        }
        for (j = n - ncancel; j < n; j++) {
            out[j] = 0;
        }
    }
    return ncancel;
}


/* -------- mp_imul routines -------- */


void mp_imul(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[])
{
    void mp_unsgn_imul(fft_int n, fft_float dradix, fft_int in1[], fft_float din2, 
            fft_int out[]);
    
    if (in2 > 0) {
        out[0] = in1[0];
    } else if (in2 < 0) {
        out[0] = -in1[0];
        in2 = -in2;
    } else {
        out[0] = 0;
    }
    mp_unsgn_imul(n, radix, &in1[1], in2, &out[1]);
    if (out[0] == 0) {
        out[1] = 0;
    }
}


fft_int mp_idiv(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[])
{
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    void mp_unsgn_idiv(fft_int n, fft_float dradix, fft_int in1[], fft_float din2, 
            fft_int out[]);
    
    if (in2 == 0) {
        return -1;
    }
    if (in2 > 0) {
        out[0] = in1[0];
    } else {
        out[0] = -in1[0];
        in2 = -in2;
    }
    if (in1[0] == 0) {
        mp_load_0(n, radix, out);
        return 0;
    }
    mp_unsgn_idiv(n, radix, &in1[1], in2, &out[1]);
    return 0;
}


void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[])
{
    fft_int j, ix, carry, shift;
    dgt_int *indgt, *outdgt;
    
    indgt = (dgt_int *) &in[2];
    outdgt = (dgt_int *) &out[2];
    out[0] = in[0];
    shift = 0;
    if (indgt[0] == 1) {
        shift = 1;
    }
    out[1] = in[1] - shift;
    carry = -shift;
    for (j = 0; j < n - shift; j++) {
        ix = indgt[j + shift] + (radix & carry);
        carry = -(ix & 1);
        outdgt[j] = (dgt_int) (ix >> 1);
    }
    if (shift > 0) {
        outdgt[n - 1] = (dgt_int) ((radix & carry) >> 1);
    }
}


/* -------- mp_imul child routines -------- */


void mp_unsgn_imul(fft_int n, fft_float dradix, fft_int in1[], fft_float din2, 
        fft_int out[])
{
    fft_int j, carry, shift;
    fft_float x, d1_radix;
    dgt_int *in1dgt, *outdgt;
    
    in1dgt = (dgt_int *) &in1[1];
    outdgt = (dgt_int *) &out[1];
    d1_radix = (fft_float) 1 / dradix;
    carry = 0;
    for (j = n - 1; j >= 0; j--) {
        x = din2 * in1dgt[j] + carry + FC_HALF;
        carry = (fft_int) (d1_radix * x);
        outdgt[j] = (dgt_int) (x - dradix * carry);
    }
    shift = 0;
    x = carry + FC_HALF;
    while (x > 1) {
        x *= d1_radix;
        shift++;
    }
    out[0] = in1[0] + shift;
    if (shift > 0) {
        while (shift > n) {
            carry = (fft_int) (d1_radix * carry + FC_HALF);
            shift--;
        }
        for (j = n - 1; j >= shift; j--) {
            outdgt[j] = outdgt[j - shift];
        }
        for (j = shift - 1; j >= 0; j--) {
            x = carry + FC_HALF;
            carry = (fft_int) (d1_radix * x);
            outdgt[j] = (dgt_int) (x - dradix * carry);
        }
    }
}


void mp_unsgn_idiv(fft_int n, fft_float dradix, fft_int in1[], fft_float din2, 
        fft_int out[])
{
    fft_int j, ix, carry, shift;
    fft_float x, d1_in2;
    dgt_int *in1dgt, *outdgt;
    
    in1dgt = (dgt_int *) &in1[1];
    outdgt = (dgt_int *) &out[1];
    d1_in2 = (fft_float) 1 / din2;
    shift = -1;
    x = 0;
    do {
        shift++;
        x *= dradix;
        if (shift < n) {
            x += in1dgt[shift];
        }
    } while (x < din2 - FC_HALF);
    x += FC_HALF;
    ix = (fft_int) (d1_in2 * x);
    carry = (fft_int) (x - din2 * ix);
    outdgt[0] = (dgt_int) ix;
    out[0] = in1[0] - shift;
    if (shift >= n) {
        shift = n - 1;
    }
    for (j = 1; j < n - shift; j++) {
        x = in1dgt[j + shift] + dradix * carry + FC_HALF;
        ix = (fft_int) (d1_in2 * x);
        carry = (fft_int) (x - din2 * ix);
        outdgt[j] = (dgt_int) ix;
    }
    for (j = n - shift; j < n; j++) {
        x = dradix * carry + FC_HALF;
        ix = (fft_int) (d1_in2 * x);
        carry = (fft_int) (x - din2 * ix);
        outdgt[j] = (dgt_int) ix;
    }
}


/* -------- mp_mul routines -------- */


fft_float mp_mul_radix_test(fft_int n, fft_int radix, fft_int nfft, 
        fft_float tmpfft[])
{
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_csqu(fft_int nfft, fft_float dinout[]);
    fft_float mp_mul_d2i_test(fft_int radix, fft_int nfft, fft_float din[]);
    fft_int j, ndata, radix_2;
    
    ndata = (nfft >> 1) + 1;
    if (ndata > n) {
        ndata = n;
    }
    tmpfft[nfft + 1] = radix - 1;
    for (j = nfft; j > ndata; j--) {
        tmpfft[j] = 0;
    }
    radix_2 = (radix + 1) / 2;
    for (j = ndata; j > 2; j--) {
        tmpfft[j] = radix_2;
    }
    tmpfft[2] = radix;
    tmpfft[1] = radix - 1;
    tmpfft[0] = 0;
    rdft(nfft, 1, &tmpfft[1]);
    mp_mul_csqu(nfft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    return mp_mul_d2i_test(radix, nfft, tmpfft);
}


void mp_mul(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[])
{
    void f_swap_in(fft_int bytesize, char *f_name, char *in);
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_f_cmul(fft_int nfft, char *f_name_in, fft_float dinout[]);
    void mp_mul_f_cmul_swap(fft_int nfft, char *f_name_inout, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift1, shift2;
    char *f_name_tmp1fft = SWAP_FILE_FFT1;
    char *f_name_tmp1 = SWAP_FILE_VAR1;
    dgt_int *in1dgt, *in2dgt;
    
    in1dgt = (dgt_int *) &in1[2];
    in2dgt = (dgt_int *) &in2[2];
    shift1 = (nfft >> 1) + 1;
    shift2 = shift1;
    while (n > shift1) {
        if (in1dgt[shift1] != 0) {
            break;
        }
        shift1++;
    }
    while (n > shift2) {
        if (in2dgt[shift2] != 0) {
            break;
        }
        shift2++;
    }
    n_h = n / 2 + 1;
    if (n_h < n - shift1) {
        n_h = n - shift1;
    }
    if (n_h < n - shift2) {
        n_h = n - shift2;
    }
    /* ---- tmp1 = (upper) in1 * (lower) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in1, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    f_swap_out((nfft + 2) * sizeof(fft_float), 
            f_name_tmp1fft, (char *) tmpfft);
    mp_mul_i2d(n, radix, nfft, shift2, in2, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    mp_mul_f_cmul(nfft, f_name_tmp1fft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, tmp);
    f_swap_out(2 * sizeof(fft_int) + n_h * sizeof(dgt_int), 
            f_name_tmp1, (char *) tmp);
    /* ---- tmp = (upper) in1 * (upper) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in2, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    mp_mul_f_cmul_swap(nfft, f_name_tmp1fft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    mp_mul_d2i(n, radix, nfft, tmpfft, tmp);
    /* ---- out = tmp + tmp1 + (upper) in2 * (lower) in1 ---- */
    mp_mul_i2d(n, radix, nfft, shift1, in1, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    mp_mul_f_cmul(nfft, f_name_tmp1fft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, out);
    if (out[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    } else {
        mp_copy(n, radix, tmp, out);
    }
    f_swap_in(2 * sizeof(fft_int) + n_h * sizeof(dgt_int), 
            f_name_tmp1, (char *) tmp);
    if (tmp[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    }
    remove(f_name_tmp1fft);
    remove(f_name_tmp1);
}


void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
        fft_int nfft, fft_float tmpfft[])
{
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_f_cmul(fft_int nfft, char *f_name_in, fft_float dinout[]);
    void mp_mul_csqu(fft_int nfft, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
    char *f_name_tmp1fft = SWAP_FILE_FFT1;
    dgt_int *indgt;
    
    indgt = (dgt_int *) &in[2];
    shift = (nfft >> 1) + 1;
    while (n > shift) {
        if (indgt[shift] != 0) {
            break;
        }
        shift++;
    }
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp = tmp + ((upper) in)^2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    f_swap_out((nfft + 2) * sizeof(fft_float), 
            f_name_tmp1fft, (char *) tmpfft);
    mp_mul_csqu(nfft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    mp_mul_d2i(n, radix, nfft, tmpfft, tmp);
    /* ---- out = (upper) in * (lower) in ---- */
    mp_mul_i2d(n, radix, nfft, shift, in, tmpfft);
    rdft(nfft, 1, &tmpfft[1]);
    mp_mul_f_cmul(nfft, f_name_tmp1fft, tmpfft);
    rdft(nfft, -1, &tmpfft[1]);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, out);
    /* ---- out = 2 * out + tmp ---- */
    if (out[0] != 0) {
        mp_add(n_h, radix, out, out, out);
        mp_add(n, radix, out, tmp, out);
    } else {
        mp_copy(n, radix, tmp, out);
    }
    remove(f_name_tmp1fft);
}


void mp_mulh_f(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int nfft, char *f_name_in1fft, fft_float outfft[])
{
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_f_cmul(fft_int nfft, char *f_name_in, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in1, outfft);
    rdft(nfft, 1, &outfft[1]);
    f_swap_out((nfft + 2) * sizeof(fft_float), 
            f_name_in1fft, (char *) outfft);
    mp_mul_i2d(n, radix, nfft, 0, in2, outfft);
    rdft(nfft, 1, &outfft[1]);
    mp_mul_f_cmul(nfft, f_name_in1fft, outfft);
    rdft(nfft, -1, &outfft[1]);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_mulh_f_use_in1fft(fft_int n, fft_int radix, char *f_name_in1fft, 
        fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[])
{
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_f_cmul(fft_int nfft, char *f_name_in, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h;
    dgt_int *in2dgt;
    
    in2dgt = (dgt_int *) &in2[2];
    while (n > shift) {
        if (in2dgt[shift] != 0) {
            break;
        }
        shift++;
    }
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    mp_mul_i2d(n, radix, nfft, shift, in2, outfft);
    rdft(nfft, 1, &outfft[1]);
    mp_mul_f_cmul(nfft, f_name_in1fft, outfft);
    rdft(nfft, -1, &outfft[1]);
    mp_mul_d2i(n_h, radix, nfft, outfft, out);
}


void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, fft_float outfft[])
{
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_csqu(fft_int nfft, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in, outfft);
    rdft(nfft, 1, &outfft[1]);
    mp_mul_csqu(nfft, outfft);
    rdft(nfft, -1, &outfft[1]);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_squh_f_save_infft(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, char *f_name_infft, fft_float outfft[])
{
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_csqu(fft_int nfft, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in, outfft);
    rdft(nfft, 1, &outfft[1]);
    f_swap_out((nfft + 2) * sizeof(fft_float), 
            f_name_infft, (char *) outfft);
    mp_mul_csqu(nfft, outfft);
    rdft(nfft, -1, &outfft[1]);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_squh_use_in1fft(fft_int n, fft_int radix, fft_float inoutfft[], fft_int out[], 
        fft_int nfft)
{
    void rdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_csqu(fft_int nfft, fft_float dinout[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_csqu(nfft, inoutfft);
    rdft(nfft, -1, &inoutfft[1]);
    mp_mul_d2i(n, radix, nfft, inoutfft, out);
}


/* -------- mp_mul child routines -------- */


void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
        fft_int in[], fft_float dout[])
{
    fft_int j, x, carry, ndata, radix_2, topdgt;
    dgt_int *indgt;
    
    indgt = (dgt_int *) &in[2];
    ndata = 0;
    topdgt = 0;
    if (n > shift) {
        topdgt = indgt[shift];
        ndata = (nfft >> 1) + 1;
        if (ndata > n - shift) {
            ndata = n - shift;
        }
    }
    dout[nfft + 1] = in[0] * topdgt;
    for (j = nfft; j > ndata; j--) {
        dout[j] = 0;
    }
    /* ---- abs(dout[j]) <= radix/2 (to keep FFT precision) ---- */
    if (ndata > 1) {
        radix_2 = radix / 2;
        carry = 0;
        for (j = ndata - 1; j > 1; j--) {
            x = indgt[j + shift] - carry;
            carry = x >= radix_2 ? -1 : 0;
            dout[j + 1] = x - (radix & carry);
        }
        dout[2] = indgt[shift + 1] - carry;
    }
    dout[1] = topdgt;
    dout[0] = in[1] - shift;
}


void mp_mul_f_cmul(fft_int nfft, char *f_name_in, fft_float dinout[])
{
    fft_int j, k;
    fft_float xr, xi, yr, yi, din[3];
    char *cin;
    FILE *f_in;
    
    cin = (char *) din;
    f_in = fopen(f_name_in, "rb");
    for (k = 0; k < 3 * (fft_int) sizeof(fft_float); k++) {
        cin[k] = (char) getc(f_in);
    }
    dinout[0] += din[0];
    dinout[1] *= din[1];
    dinout[2] *= din[2];
    for (j = 3; j < nfft; j += 2) {
        for (k = 0; k < 2 * (fft_int) sizeof(fft_float); k++) {
            cin[k] = (char) getc(f_in);
        }
        xr = din[0];
        xi = din[1];
        yr = dinout[j];
        yi = dinout[j + 1];
        dinout[j] = xr * yr - xi * yi;
        dinout[j + 1] = xr * yi + xi * yr;
    }
    for (k = 0; k < (fft_int) sizeof(fft_float); k++) {
        cin[k] = (char) getc(f_in);
    }
    dinout[nfft + 1] *= din[0];
    fclose(f_in);
}


void mp_mul_f_cmul_swap(fft_int nfft, char *f_name_inout, fft_float dinout[])
{
    fft_int n_j, j, k;
    fft_float xr, xi, yr, yi, din[2 * F_CMUL_BUF + 1];
    char *cin, *cout;
    FILE *f_inout;
    
    f_inout = fopen(f_name_inout, "rb+");
    setvbuf(f_inout, NULL, _IOFBF, 
            (fft_int) sizeof(fft_float) * (2 * F_CMUL_BUF + 1));
    cin = (char *) din;
    for (k = 0; k < 3 * (fft_int) sizeof(fft_float); k++) {
        cin[k] = (char) getc(f_inout);
    }
    cout = (char *) dinout;
    fseek(f_inout, -3 * (fft_int) sizeof(fft_float), SEEK_CUR);
    for (k = 0; k < 3 * (fft_int) sizeof(fft_float); k++) {
        putc((fft_int) cout[k], f_inout);
    }
    fflush(f_inout);
    dinout[0] += din[0];
    dinout[1] *= din[1];
    dinout[2] *= din[2];
    j = 3;
    while (j < nfft + 1 - 2 * F_CMUL_BUF) {
        for (k = 0; k < 2 * F_CMUL_BUF * (fft_int) sizeof(fft_float); k++) {
            cin[k] = (char) getc(f_inout);
        }
        cout = (char *) (&dinout[j]);
        fseek(f_inout, -2 * F_CMUL_BUF * (fft_int) sizeof(fft_float), SEEK_CUR);
        for (k = 0; k < 2 * F_CMUL_BUF * (fft_int) sizeof(fft_float); k++) {
            putc((fft_int) cout[k], f_inout);
        }
        fflush(f_inout);
        for (k = 0; k < 2 * F_CMUL_BUF; k += 2) {
            xr = din[k];
            xi = din[k + 1];
            yr = dinout[j + k];
            yi = dinout[j + k + 1];
            dinout[j + k] = xr * yr - xi * yi;
            dinout[j + k + 1] = xr * yi + xi * yr;
        }
        j += 2 * F_CMUL_BUF;
    }
    n_j = nfft + 1 - j;
    if (n_j > 0) {
        for (k = 0; k < n_j * (fft_int) sizeof(fft_float); k++) {
            cin[k] = (char) getc(f_inout);
        }
        cout = (char *) (&dinout[j]);
        fseek(f_inout, -n_j * (fft_int) sizeof(fft_float), SEEK_CUR);
        for (k = 0; k < n_j * (fft_int) sizeof(fft_float); k++) {
            putc((fft_int) cout[k], f_inout);
        }
        fflush(f_inout);
        for (k = 0; k < n_j; k += 2) {
            xr = din[k];
            xi = din[k + 1];
            yr = dinout[j + k];
            yi = dinout[j + k + 1];
            dinout[j + k] = xr * yr - xi * yi;
            dinout[j + k + 1] = xr * yi + xi * yr;
        }
    }
    for (k = 0; k < (fft_int) sizeof(fft_float); k++) {
        cin[k] = (char) getc(f_inout);
    }
    cout = (char *) (&dinout[nfft + 1]);
    fseek(f_inout, -((fft_int) sizeof(fft_float)), SEEK_CUR);
    for (k = 0; k < (fft_int) sizeof(fft_float); k++) {
        putc((fft_int) cout[k], f_inout);
    }
    dinout[nfft + 1] *= din[0];
    fclose(f_inout);
}


void mp_mul_f_cmuladd(fft_int nfft, fft_float dinout[], 
        char *f_name_in2, char *f_name_in3)
{
    fft_int j, k;
    fft_float xr, xi, yr, yi, din2[3], din3[3];
    char *cin2, *cin3;
    FILE *f_in2, *f_in3;
    
    cin2 = (char *) din2;
    cin3 = (char *) din3;
    f_in2 = fopen(f_name_in2, "rb");
    f_in3 = fopen(f_name_in3, "rb");
    for (k = 0; k < 3 * (fft_int) sizeof(fft_float); k++) {
        cin2[k] = (char) getc(f_in2);
        cin3[k] = (char) getc(f_in3);
    }
    dinout[0] = din3[0];
    dinout[1] = din3[1] + dinout[1] * din2[1];
    dinout[2] = din3[2] + dinout[2] * din2[2];
    for (j = 3; j < nfft; j += 2) {
        for (k = 0; k < 2 * (fft_int) sizeof(fft_float); k++) {
            cin2[k] = (char) getc(f_in2);
            cin3[k] = (char) getc(f_in3);
        }
        xr = dinout[j];
        xi = dinout[j + 1];
        yr = din2[0];
        yi = din2[1];
        dinout[j] = din3[0] + (xr * yr - xi * yi);
        dinout[j + 1] = din3[1] + (xr * yi + xi * yr);
    }
    for (k = 0; k < (fft_int) sizeof(fft_float); k++) {
        cin2[k] = (char) getc(f_in2);
        cin3[k] = (char) getc(f_in3);
    }
    dinout[nfft + 1] = din3[0] + dinout[nfft + 1] * din2[0];
    fclose(f_in2);
    fclose(f_in3);
}


void mp_mul_csqu(fft_int nfft, fft_float dinout[])
{
    fft_int j;
    fft_float xr, xi;
    
    dinout[0] *= 2;
    dinout[1] *= dinout[1];
    dinout[2] *= dinout[2];
    for (j = 3; j < nfft; j += 2) {
        xr = dinout[j];
        xi = dinout[j + 1];
        dinout[j] = xr * xr - xi * xi;
        dinout[j + 1] = 2 * xr * xi;
    }
    dinout[nfft + 1] *= dinout[nfft + 1];
}


void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[])
{
    fft_int j, carry, carry1, carry2, shift, ndata;
    fft_float x, scale, d1_radix, d1_radix2, pow_radix, topdgt;
    dgt_int *outdgt;
    
    outdgt = (dgt_int *) &out[2];
    scale = (fft_float) 2 / nfft;
    d1_radix = (fft_float) 1 / radix;
    d1_radix2 = d1_radix * d1_radix;
    topdgt = din[nfft + 1];
    x = topdgt < 0 ? -topdgt : topdgt;
    shift = x + FC_HALF >= radix ? 1 : 0;
    /* ---- correction of cyclic convolution of din[1] ---- */
    x *= nfft * FC_HALF;
    din[nfft + 1] = din[1] - x;
    din[1] = x;
    /* ---- output of digits ---- */
    ndata = n;
    if (n > nfft + 1 + shift) {
        ndata = nfft + 1 + shift;
        for (j = n - 1; j >= ndata; j--) {
            outdgt[j] = 0;
        }
    }
    x = 0;
    pow_radix = 1;
    for (j = ndata + 1 - shift; j <= nfft + 1; j++) {
        x += pow_radix * din[j];
        pow_radix *= d1_radix;
        if (pow_radix < FFT_FLOAT_EPSILON) {
            break;
        }
    }
    x = d1_radix2 * (scale * x + FC_HALF);
    carry2 = (fft_int) x - 1;
    carry = (fft_int) (radix * (x - carry2) + FC_HALF);
    for (j = ndata; j > 1; j--) {
        x = d1_radix2 * (scale * din[j - shift] + carry + FC_HALF);
        carry = carry2;
        carry2 = (fft_int) x - 1;
        x = radix * (x - carry2);
        carry1 = (fft_int) x;
        outdgt[j - 1] = (dgt_int) (radix * (x - carry1));
        carry += carry1;
    }
    x = carry + (fft_float) radix * carry2 + FC_HALF;
    if (shift == 0) {
        x += scale * din[1];
    }
    carry = (fft_int) (d1_radix * x);
    outdgt[0] = (dgt_int) (x - (fft_float) radix * carry);
    if (carry > 0) {
        for (j = n - 1; j > 0; j--) {
            outdgt[j] = outdgt[j - 1];
        }
        outdgt[0] = (dgt_int) carry;
        shift++;
    }
    /* ---- output of exp, sgn ---- */
    x = din[0] + shift + FC_HALF;
    shift = (fft_int) x - 1;
    out[1] = shift + (fft_int) (x - shift);
    out[0] = topdgt > FC_HALF ? 1 : -1;
    if (outdgt[0] == 0) {
        out[0] = 0;
        out[1] = 0;
    }
}


fft_float mp_mul_d2i_test(fft_int radix, fft_int nfft, fft_float din[])
{
    fft_int j, carry, carry1, carry2;
    fft_float x, scale, d1_radix, d1_radix2, err;
    
    scale = (fft_float) 2 / nfft;
    d1_radix = (fft_float) 1 / radix;
    d1_radix2 = d1_radix * d1_radix;
    /* ---- correction of cyclic convolution of din[1] ---- */
    x = din[nfft + 1] * nfft * FC_HALF;
    if (x < 0) {
        x = -x;
    }
    din[nfft + 1] = din[1] - x;
    /* ---- check of digits ---- */
    err = 0;
    carry = 0;
    carry2 = 0;
    for (j = nfft + 1; j > 1; j--) {
        x = d1_radix2 * (scale * din[j] + carry + FC_HALF);
        carry = carry2;
        carry2 = (fft_int) x - 1;
        x = radix * (x - carry2);
        carry1 = (fft_int) x;
        x = radix * (x - carry1);
        carry += carry1;
        x = x - FC_HALF - (fft_int) x;
        if (x > err) {
            err = x;
        } else if (-x > err) {
            err = -x;
        }
    }
    return err;
}


/* -------- mp_inv routines -------- */


fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[])
{
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_inv_init(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    fft_int mp_inv_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
            fft_int tmp[], char *f_name_tmp1, fft_int nfft, fft_float tmpfft[], 
            char *f_name_tmp1fft);
    fft_int n_nwt, nfft_nwt, thr, prc;
    char *f_name_tmp1fft = SWAP_FILE_FFT1;
    char *f_name_tmp1 = SWAP_FILE_VAR1;
    
    if (in[0] == 0) {
        return -1;
    }
    nfft_nwt = mp_get_nfft_init(radix, nfft);
    n_nwt = nfft_nwt + 2;
    if (n_nwt > n) {
        n_nwt = n;
    }
    mp_inv_init(n_nwt, radix, in, out);
    thr = 8;
    do {
        n_nwt = nfft_nwt + 2;
        if (n_nwt > n) {
            n_nwt = n;
        }
        prc = mp_inv_newton(n_nwt, radix, in, out, 
                tmp, f_name_tmp1, nfft_nwt, tmpfft, f_name_tmp1fft);
        if (thr * nfft_nwt >= nfft) {
            thr = 0;
            if (2 * prc <= n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        } else {
            if (3 * prc < n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        }
        nfft_nwt <<= 1;
    } while (nfft_nwt <= nfft);
    remove(f_name_tmp1fft);
    remove(f_name_tmp1);
    return 0;
}


fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmpfft[])
{
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_sqrt_init(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int out_rev[]);
    fft_int mp_sqrt_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
            fft_int inout_rev[], char *f_name_tmp1, char *f_name_tmp2, 
            fft_int nfft, fft_float tmpfft[], char *f_name_tmp1fft, fft_int *n_tmp1fft);
    fft_int n_nwt, nfft_nwt, thr, prc, n_tmp1fft;
    char *f_name_tmp1fft = SWAP_FILE_FFT1;
    char *f_name_tmp1 = SWAP_FILE_VAR1;
    char *f_name_tmp2 = SWAP_FILE_VAR2;
    
    if (in[0] < 0) {
        return -1;
    } else if (in[0] == 0) {
        mp_load_0(n, radix, out);
        return 0;
    }
    nfft_nwt = mp_get_nfft_init(radix, nfft);
    n_nwt = nfft_nwt + 2;
    if (n_nwt > n) {
        n_nwt = n;
    }
    mp_sqrt_init(n_nwt, radix, in, out, tmp);
    n_tmp1fft = 0;
    thr = 8;
    do {
        n_nwt = nfft_nwt + 2;
        if (n_nwt > n) {
            n_nwt = n;
        }
        prc = mp_sqrt_newton(n_nwt, radix, in, out, 
                tmp, f_name_tmp1, f_name_tmp2, nfft_nwt, tmpfft, 
                f_name_tmp1fft, &n_tmp1fft);
        if (thr * nfft_nwt >= nfft) {
            thr = 0;
            if (2 * prc <= n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        } else {
            if (3 * prc < n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        }
        nfft_nwt <<= 1;
    } while (nfft_nwt <= nfft);
    remove(f_name_tmp1fft);
    remove(f_name_tmp1);
    remove(f_name_tmp2);
    return 0;
}


fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmpfft[])
{
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_invisqrt_init(fft_int n, fft_int radix, fft_int in, fft_int out[]);
    fft_int mp_invisqrt_newton(fft_int n, fft_int radix, fft_int in, fft_int inout[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmpfft[], 
            char *f_name_tmp1fft);
    fft_int n_nwt, nfft_nwt, thr, prc;
    char *f_name_tmp1fft = SWAP_FILE_FFT1;
    
    if (in <= 0) {
        return -1;
    }
    nfft_nwt = mp_get_nfft_init(radix, nfft);
    n_nwt = nfft_nwt + 2;
    if (n_nwt > n) {
        n_nwt = n;
    }
    mp_invisqrt_init(n_nwt, radix, in, out);
    thr = 8;
    do {
        n_nwt = nfft_nwt + 2;
        if (n_nwt > n) {
            n_nwt = n;
        }
        prc = mp_invisqrt_newton(n_nwt, radix, in, out, 
                tmp1, tmp2, nfft_nwt, tmpfft, f_name_tmp1fft);
        if (thr * nfft_nwt >= nfft) {
            thr = 0;
            if (2 * prc <= n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        } else {
            if (3 * prc < n_nwt - 2) {
                nfft_nwt >>= 1;
            }
        }
        nfft_nwt <<= 1;
    } while (nfft_nwt <= nfft);
    remove(f_name_tmp1fft);
    return 0;
}


/* -------- mp_inv child routines -------- */


fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max)
{
    fft_int nfft_init;
    fft_float r;
    
    r = radix;
    nfft_init = 1;
    do {
        r *= r;
        nfft_init <<= 1;
    } while (FFT_FLOAT_EPSILON * r < 1 && nfft_init < nfft_max);
    return nfft_init;
}


void mp_inv_init(fft_int n, fft_int radix, fft_int in[], fft_int out[])
{
    void mp_unexp_d2mp(fft_int n, fft_int radix, fft_float din, dgt_int out[]);
    fft_float mp_unexp_mp2d(fft_int n, fft_int radix, dgt_int in[]);
    fft_int outexp;
    fft_float din;
    
    out[0] = in[0];
    outexp = -in[1];
    din = (fft_float) 1 / mp_unexp_mp2d(n, radix, (dgt_int *) &in[2]);
    while (din < 1) {
        din *= radix;
        outexp--;
    }
    out[1] = outexp;
    mp_unexp_d2mp(n, radix, din, (dgt_int *) &out[2]);
}


void mp_sqrt_init(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int out_rev[])
{
    void mp_unexp_d2mp(fft_int n, fft_int radix, fft_float din, dgt_int out[]);
    fft_float mp_unexp_mp2d(fft_int n, fft_int radix, dgt_int in[]);
    fft_int outexp;
    fft_float din;
    
    out[0] = 1;
    out_rev[0] = 1;
    outexp = in[1];
    din = mp_unexp_mp2d(n, radix, (dgt_int *) &in[2]);
    if (outexp % 2 != 0) {
        din *= radix;
        outexp--;
    }
    outexp /= 2;
    din = fft_sqrt(din);
    if (din < 1) {
        din *= radix;
        outexp--;
    }
    out[1] = outexp;
    mp_unexp_d2mp(n, radix, din, (dgt_int *) &out[2]);
    outexp = -outexp;
    din = (fft_float) 1 / din;
    while (din < 1) {
        din *= radix;
        outexp--;
    }
    out_rev[1] = outexp;
    mp_unexp_d2mp(n, radix, din, (dgt_int *) &out_rev[2]);
}


void mp_invisqrt_init(fft_int n, fft_int radix, fft_int in, fft_int out[])
{
    void mp_unexp_d2mp(fft_int n, fft_int radix, fft_float din, dgt_int out[]);
    fft_int outexp;
    fft_float dout;
    
    out[0] = 1;
    outexp = 0;
    dout = fft_sqrt((fft_float) 1 / in);
    while (dout < 1) {
        dout *= radix;
        outexp--;
    }
    out[1] = outexp;
    mp_unexp_d2mp(n, radix, dout, (dgt_int *) &out[2]);
}


void mp_unexp_d2mp(fft_int n, fft_int radix, fft_float din, dgt_int out[])
{
    fft_int j, x;
    
    for (j = 0; j < n; j++) {
        x = (fft_int) din;
        if (x >= radix) {
            x = radix - 1;
            din = radix;
        }
        din = radix * (din - x);
        out[j] = (dgt_int) x;
    }
}


fft_float mp_unexp_mp2d(fft_int n, fft_int radix, dgt_int in[])
{
    fft_int j;
    fft_float d1_radix, dout;
    
    d1_radix = (fft_float) 1 / radix;
    dout = 0;
    for (j = n - 1; j >= 0; j--) {
        dout = d1_radix * dout + in[j];
    }
    return dout;
}


fft_int mp_inv_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
        fft_int tmp[], char *f_name_tmp1, fft_int nfft, fft_float tmpfft[], 
        char *f_name_tmp1fft)
{
    void f_swap_in(fft_int bytesize, char *f_name, char *in);
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mulh_f(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int nfft, char *f_name_in1fft, fft_float outfft[]);
    void mp_mulh_f_use_in1fft(fft_int n, fft_int radix, char *f_name_in1fft, 
            fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[]);
    fft_int n_h, shift, prc;
    
    shift = (nfft >> 1) + 1;
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp1 = inout ---- */
    mp_round(n, radix, shift, inout);
    f_swap_out(2 * sizeof(fft_int) + shift * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout);
    /* ---- tmp = tmp1 * (upper) in (half to normal precision) ---- */
    mp_mulh_f(n, radix, inout, in, tmp, 
            nfft, f_name_tmp1fft, tmpfft);
    /* ---- tmp = 1 - tmp ---- */
    mp_load_1(n, radix, inout);
    mp_sub(n, radix, inout, tmp, tmp);
    /* ---- tmp -= tmp1 * (lower) in (half precision) ---- */
    mp_mulh_f_use_in1fft(n, radix, f_name_tmp1fft, shift, in, inout, 
            nfft, tmpfft);
    mp_sub(n_h, radix, tmp, inout, tmp);
    /* ---- get precision ---- */
    prc = -tmp[1];
    if (tmp[0] == 0) {
        prc = nfft + 1;
    }
    /* ---- tmp *= tmp1 (half precision) ---- */
    mp_mulh_f_use_in1fft(n_h, radix, f_name_tmp1fft, 0, tmp, tmp, 
            nfft, tmpfft);
    /* ---- inout = tmp1 + tmp ---- */
    mp_load_0(n, radix, inout);
    f_swap_in(2 * sizeof(fft_int) + shift * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout);
    if (tmp[0] != 0) {
        mp_add(n, radix, inout, tmp, inout);
    }
    return prc;
}


fft_int mp_sqrt_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
        fft_int inout_rev[], char *f_name_tmp1, char *f_name_tmp2, 
        fft_int nfft, fft_float tmpfft[], char *f_name_tmp1fft, fft_int *n_tmp1fft)
{
    void f_swap_in(fft_int bytesize, char *f_name, char *in);
    void f_swap_out(fft_int bytesize, char *f_name, char *out);
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_mulh_f(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int nfft, char *f_name_in1fft, fft_float outfft[]);
    void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int nfft, fft_float inoutfft[]);
    void mp_squh_use_in1fft(fft_int n, fft_int radix, fft_float inoutfft[], fft_int out[], 
            fft_int nfft);
    fft_int n_h, nfft_h, shift, prc;
    
    nfft_h = nfft >> 1;
    shift = nfft_h + 1;
    if (nfft_h < 2) {
        nfft_h = 2;
    }
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp1 = inout_rev ---- */
    mp_round(n_h, radix, (nfft_h >> 1) + 1, inout_rev);
    f_swap_out(2 * sizeof(fft_int) + ((nfft_h >> 1) + 1) * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout_rev);
    /* ---- inout_rev = tmp1^2 (1/4 to half precision) ---- */
    if (*n_tmp1fft != nfft_h) {
        mp_squh(n_h, radix, inout_rev, inout_rev, 
                nfft_h, tmpfft);
    } else {
        f_swap_in((nfft_h + 2) * sizeof(fft_float), 
                f_name_tmp1fft, (char *) tmpfft);
        mp_squh_use_in1fft(n_h, radix, tmpfft, inout_rev, 
                nfft_h);
    }
    /* ---- tmp2 = inout ---- */
    mp_round(n, radix, shift, inout);
    f_swap_out(2 * sizeof(fft_int) + shift * sizeof(dgt_int), 
            f_name_tmp2, (char *) inout);
    /* ---- tmp1 += tmp1 - tmp2 * inout_rev (half precision) ---- */
    mp_mulh_f(n_h, radix, inout, inout_rev, inout, 
            nfft, f_name_tmp1fft, tmpfft);
    mp_load_0(n_h, radix, inout_rev);
    f_swap_in(2 * sizeof(fft_int) + ((nfft_h >> 1) + 1) * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout_rev);
    mp_sub(n_h, radix, inout_rev, inout, inout);
    mp_add(n_h, radix, inout_rev, inout, inout_rev);
    mp_round(n_h, radix, shift, inout_rev);
    f_swap_out(2 * sizeof(fft_int) + n_h * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout_rev);
    /* ---- inout = in - tmp2^2 (half to normal precision) ---- */
    f_swap_in((nfft + 2) * sizeof(fft_float), 
            f_name_tmp1fft, (char *) tmpfft);
    mp_squh_use_in1fft(n, radix, tmpfft, inout, nfft);
    mp_sub(n, radix, in, inout, inout);
    /* ---- get precision ---- */
    prc = in[1] - inout[1];
    if (((dgt_int *) &in[2])[0] > ((dgt_int *) &inout[2])[0]) {
        prc++;
    }
    if (inout[0] == 0) {
        prc = nfft + 1;
    }
    /* ---- inout = tmp2 + tmp1 * inout / 2 (half precision) ---- */
    mp_mulh_f(n_h, radix, inout_rev, inout, inout_rev, 
            nfft, f_name_tmp1fft, tmpfft);
    *n_tmp1fft = nfft;
    mp_idiv_2(n_h, radix, inout_rev, inout_rev);
    mp_load_0(n, radix, inout);
    f_swap_in(2 * sizeof(fft_int) + shift * sizeof(dgt_int), 
            f_name_tmp2, (char *) inout);
    if (inout_rev[0] != 0) {
        mp_add(n, radix, inout, inout_rev, inout);
    }
    /* ---- inout_rev = tmp1 ---- */
    f_swap_in(2 * sizeof(fft_int) + n_h * sizeof(dgt_int), 
            f_name_tmp1, (char *) inout_rev);
    return prc;
}


fft_int mp_invisqrt_newton(fft_int n, fft_int radix, fft_int in, fft_int inout[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmpfft[], 
        char *f_name_tmp1fft)
{
    void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_imul(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
    void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_squh_f_save_infft(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int nfft, char *f_name_infft, fft_float outfft[]);
    void mp_mulh_f_use_in1fft(fft_int n, fft_int radix, char *f_name_in1fft, 
            fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[]);
    fft_int n_h, shift, prc;
    
    shift = (nfft >> 1) + 1;
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp1 = in * inout^2 (half to normal precision) ---- */
    mp_round(n, radix, shift, inout);
    mp_squh_f_save_infft(n, radix, inout, tmp1, 
            nfft, f_name_tmp1fft, tmpfft);
    mp_imul(n, radix, tmp1, in, tmp1);
    /* ---- tmp2 = 1 - tmp1 ---- */
    mp_load_1(n, radix, tmp2);
    mp_sub(n, radix, tmp2, tmp1, tmp2);
    /* ---- get precision ---- */
    prc = -tmp2[1];
    if (tmp2[0] == 0) {
        prc = nfft + 1;
    }
    /* ---- tmp2 *= inout / 2 (half precision) ---- */
    mp_mulh_f_use_in1fft(n_h, radix, f_name_tmp1fft, 0, tmp2, tmp2, 
            nfft, tmpfft);
    mp_idiv_2(n_h, radix, tmp2, tmp2);
    /* ---- inout += tmp2 ---- */
    if (tmp2[0] != 0) {
        mp_add(n, radix, inout, tmp2, inout);
    }
    return prc;
}


/* -------- mp_io routines -------- */


void mp_sprintf(fft_int n, fft_int log10_radix, fft_int in[], char out[])
{
    fft_int j, k, x, y, outexp, shift;
    dgt_int *indgt;
    
    indgt = (dgt_int *) &in[2];
    if (in[0] < 0) {
        *out++ = '-';
    }
    x = indgt[0];
    shift = log10_radix;
    for (k = log10_radix; k > 0; k--) {
        y = x % 10;
        x /= 10;
        out[k] = '0' + y;
        if (y != 0) {
            shift = k;
        }
    }
    out[0] = out[shift];
    out[1] = '.';
    for (k = 1; k <= log10_radix - shift; k++) {
        out[k + 1] = out[k + shift];
    }
    outexp = log10_radix - shift;
    out += outexp + 2;
    for (j = 1; j < n; j++) {
        x = indgt[j];
        for (k = log10_radix - 1; k >= 0; k--) {
            y = x % 10;
            x /= 10;
            out[k] = '0' + y;
        }
        out += log10_radix;
    }
    *out++ = 'e';
    outexp += log10_radix * in[1];
    sprintf(out, "%.0f", (double) outexp);
}


void mp_sscanf(fft_int n, fft_int log10_radix, char in[], fft_int out[])
{
    char *s;
    fft_int j, x, outexp, outexp_mod;
    dgt_int *outdgt;
    double t;
    
    outdgt = (dgt_int *) &out[2];
    while (*in == ' ') {
        in++;
    }
    out[0] = 1;
    if (*in == '-') {
        out[0] = -1;
        in++;
    } else if (*in == '+') {
        in++;
    }
    while (*in == ' ' || *in == '0') {
        in++;
    }
    outexp = 0;
    for (s = in; *s != '\0'; s++) {
        if (*s == 'e' || *s == 'E' || *s == 'd' || *s == 'D') {
            if (sscanf(++s, "%lg", &t) != 1) {
                outexp = 0;
            }
            outexp = (fft_int) t;
            break;
        }
    }
    if (*in == '.') {
        do {
            outexp--;
            while (*++in == ' ');
        } while (*in == '0' && *in != '\0');
    } else if (*in != '\0') {
        s = in;
        while (*++s == ' ');
        while (*s >= '0' && *s <= '9' && *s != '\0') {
            outexp++;
            while (*++s == ' ');
        }
    }
    x = outexp / log10_radix;
    outexp_mod = outexp - log10_radix * x;
    if (outexp_mod < 0) {
        x--;
        outexp_mod += log10_radix;
    }
    out[1] = x;
    x = 0;
    j = 0;
    for (s = in; *s != '\0'; s++) {
        if (*s == '.' || *s == ' ') {
            continue;
        }
        if (*s < '0' || *s > '9') {
            break;
        }
        x = 10 * x + (*s - '0');
        if (--outexp_mod < 0) {
            if (j >= n) {
                break;
            }
            outdgt[j++] = (dgt_int) x;
            x = 0;
            outexp_mod = log10_radix - 1;
        }
    }
    while (outexp_mod-- >= 0) {
        x *= 10;
    }
    while (j < n) {
        outdgt[j++] = (dgt_int) x;
        x = 0;
    }
    if (outdgt[0] == 0) {
        out[0] = 0;
        out[1] = 0;
    }
}


void mp_fprintf(fft_int n, fft_int log10_radix, fft_int in[], FILE *fout)
{
    fft_int j, k, x, y, outexp, shift;
    char out[256];
    dgt_int *indgt;
    
    indgt = (dgt_int *) &in[2];
    if (in[0] < 0) {
        putc('-', fout);
    }
    x = indgt[0];
    shift = log10_radix;
    for (k = log10_radix; k > 0; k--) {
        y = x % 10;
        x /= 10;
        out[k] = '0' + y;
        if (y != 0) {
            shift = k;
        }
    }
    putc(out[shift], fout);
    putc('.', fout);
    for (k = 1; k <= log10_radix - shift; k++) {
        putc(out[k + shift], fout);
    }
    outexp = log10_radix - shift;
    for (j = 1; j < n; j++) {
        x = indgt[j];
        for (k = log10_radix - 1; k >= 0; k--) {
            y = x % 10;
            x /= 10;
            out[k] = '0' + y;
        }
        for (k = 0; k < log10_radix; k++) {
            putc(out[k], fout);
        }
    }
    putc('e', fout);
    outexp += log10_radix * in[1];
    sprintf(out, "%.0f", (double) outexp);
    for (k = 0; out[k] != '\0'; k++) {
        putc(out[k], fout);
    }
}


fft_int mp_chksum(fft_int n, fft_int in[])
{
    fft_int j, sum;
    dgt_int *indgt;
    
    indgt = (dgt_int *) &in[2];
    sum = in[0] ^ in[1];
    for (j = 0; j < n; j++) {
        sum ^= (fft_int) indgt[j];
    }
    return sum;
}


