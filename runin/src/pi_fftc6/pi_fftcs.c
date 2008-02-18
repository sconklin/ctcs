/*
---- calculation of PI(= 3.14159...) using FFT and AGM ----
    by T.Ooura

Example compilation:
    GNU      : gcc -O6 -ffast-math pi_fftcs.c fftsg_hx.c -lm -o pi_css
    SUN      : cc -fast -xO5 pi_fftcs.c fftsg_hx.c -lm -o pi_css
    Microsoft: cl /O2 /G6 pi_fftcs.c fftsg_hx.c /Fepi_css.exe
    ...
    etc.
*/

#include "config.h"
#define PI_FFTC_VER "ver. LG1.1.2-MP1.5.2af.memsave"


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
        fft_int tmp[], fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[], 
        fft_float tmp3fft[]);
void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
        fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[]);
void mp_mulhf(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float in1fft[], fft_float tmpfft[]);
void mp_mulhf_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], fft_int in2[], 
        fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
void mp_squhf_use_infft(fft_int n, fft_int radix, fft_float infft[], fft_int in[], 
        fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
void mp_mulh(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int nfft, fft_float in1fft[], fft_float outfft[]);
void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, fft_float outfft[]);
fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[]);
fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[]);
fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[]);
void mp_sprintf(fft_int n, fft_int log10_radix, fft_int in[], char out[]);
void mp_sscanf(fft_int n, fft_int log10_radix, char in[], fft_int out[]);
void mp_fprintf(fft_int n, fft_int log10_radix, fft_int in[], FILE *fout);
fft_int mp_chksum(fft_int n, fft_int in[]);


int main()
{
    fft_int nfft, log2_nfft, radix, log10_radix, n, npow, nprc, sum;
    fft_float err;
    fft_int *a, *b, *c, *e, *i1, *i2;
    fft_float *d1, *d2, *d3;
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
    e = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    i1 = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    i2 = (fft_int *) malloc(2 * sizeof(fft_int) + n * sizeof(dgt_int));
    memsize = 6 * (2 * sizeof(fft_int) + n * sizeof(dgt_int));
    d1 = (fft_float *) malloc((nfft + 2) * sizeof(fft_float));
    d2 = (fft_float *) malloc((nfft + 2) * sizeof(fft_float));
    d3 = (fft_float *) malloc((nfft + 2) * sizeof(fft_float));
    memsize += 3 * ((nfft + 2) * sizeof(fft_float));
    if (d3 == NULL) {
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
    /* ---- c = 1 / sqrt(8) ---- */
    mp_invisqrt(n, radix, 8, c, i1, i2, nfft, d1, d2);
    /* ---- a = 1 + 3 * c ---- */
    mp_imul(n, radix, c, 3, e);
    mp_sscanf(n, log10_radix, "1", a);
    mp_add(n, radix, a, e, a);
    /* ---- b = sqrt(a) ---- */
    mp_sqrt(n, radix, a, b, i1, i2, nfft, d1, d2);
    /* ---- e = b - 0.625 ---- */
    mp_sscanf(n, log10_radix, "0.625", e);
    mp_sub(n, radix, b, e, e);
    /* ---- b = 2 * b ---- */
    mp_add(n, radix, b, b, b);
    /* ---- c = e - c ---- */
    mp_sub(n, radix, e, c, c);
    /* ---- a = a + e ---- */
    mp_add(n, radix, a, e, a);
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
        /* ---- e = (a + b) / 2 ---- */
        mp_add(n, radix, a, b, e);
        mp_idiv_2(n, radix, e, e);
        /* ---- b = sqrt(a * b) ---- */
        mp_mul(n, radix, a, b, a, i1, nfft, d1, d2, d3);
        mp_sqrt(n, radix, a, b, i1, i2, nfft, d1, d2);
        /* ---- e = e - b ---- */
        mp_sub(n, radix, e, b, e);
        /* ---- b = 2 * b ---- */
        mp_add(n, radix, b, b, b);
        /* ---- c = c - e ---- */
        mp_sub(n, radix, c, e, c);
        /* ---- a = e + b ---- */
        mp_add(n, radix, e, b, a);
        /* ---- convergence check ---- */
        nprc = -e[1];
        if (e[0] == 0) {
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
    /* ---- e = e * e / 4 (half precision) ---- */
    mp_idiv_2(n, radix, e, e);
    mp_squh(n, radix, e, e, nfft, d1);
    /* ---- a = a + b ---- */
    mp_add(n, radix, a, b, a);
    /* ---- a = (a * a - e - e / 2) / (a * c - e) / npow ---- */
    mp_mulhf(n, radix, a, c, c, i1, nfft, d1, d2);
    mp_sub(n, radix, c, e, c);
    mp_inv(n, radix, c, b, i1, i2, nfft, d2, d3);
    mp_squhf_use_infft(n, radix, d1, a, a, i1, nfft, d2);
    mp_sub(n, radix, a, e, a);
    mp_idiv_2(n, radix, e, e);
    mp_sub(n, radix, a, e, a);
    mp_mul(n, radix, a, b, a, i1, nfft, d1, d2, d3);
    mp_idiv(n, radix, a, npow, a);
    /* ---- time check ---- */
    time(&t_2);
    sum = mp_chksum(n, a);
    /* ---- output ---- */
    f_out = fopen("pi.dat", "w");
    printf("writing pi.dat...\n");
    mp_fprintf(n - 1, log10_radix, a, f_out);
    fclose(f_out);
    free(d3);
    free(d2);
    free(d1);
    free(i2);
    free(i1);
    free(e);
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
            fft_int tmp[], fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[], 
            fft_float tmp3fft[]);
    void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
            fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[]);
    void mp_mulhf(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int tmp[], fft_int nfft, fft_float in1fft[], fft_float tmpfft[]);
    void mp_mulhf_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], fft_int in2[], 
            fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
    void mp_squhf_use_infft(fft_int n, fft_int radix, fft_float infft[], fft_int in[], 
            fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[]);
    void mp_mulh(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int nfft, fft_float in1fft[], fft_float outfft[]);
    void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int nfft, fft_float outfft[]);
    fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
            fft_float tmp1fft[], fft_float tmp2fft[]);
    fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
            fft_float tmp1fft[], fft_float tmp2fft[]);
    fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
            fft_float tmp1fft[], fft_float tmp2fft[]);
    void mp_sprintf(fft_int n, fft_int log10_radix, fft_int in[], char out[]);
    void mp_sscanf(fft_int n, fft_int log10_radix, char in[], fft_int out[]);
    void mp_fprintf(fft_int n, fft_int log10_radix, fft_int in[], FILE *fout);
   ----
*/


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
    void mp_mul_csqu(fft_int nfft, fft_float d1[]);
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
    mp_mul_csqu(nfft, tmpfft);
    return 2 * mp_mul_d2i_test(radix, nfft, tmpfft);
}


void mp_mul(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[], 
        fft_float tmp3fft[])
{
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul_nt_out(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_cmul_nt_d2(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_cmul_nt_d1_add(fft_int nfft, fft_float d1[], fft_float d2[], 
            fft_float d3[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
    dgt_int *in1dgt, *in2dgt;
    
    in1dgt = (dgt_int *) &in1[2];
    in2dgt = (dgt_int *) &in2[2];
    shift = (nfft >> 1) + 1;
    while (n > shift) {
        if (in1dgt[shift] + in2dgt[shift] != 0) {
            break;
        }
        shift++;
    }
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp3fft = (upper) in1 * (lower) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in1, tmp1fft);
    mp_mul_i2d(n, radix, nfft, shift, in2, tmp3fft);
    mp_mul_cmul_nt_out(nfft, tmp1fft, tmp3fft);
    /* ---- tmp = (upper) in1 * (upper) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in2, tmp2fft);
    mp_mul_cmul_nt_d2(nfft, tmp2fft, tmp1fft);
    mp_mul_d2i(n, radix, nfft, tmp1fft, tmp);
    /* ---- tmp3fft += (upper) in2 * (lower) in1 ---- */
    mp_mul_i2d(n, radix, nfft, shift, in1, tmp1fft);
    mp_mul_cmul_nt_d1_add(nfft, tmp2fft, tmp1fft, tmp3fft);
    /* ---- out = tmp + tmp3fft ---- */
    mp_mul_d2i(n_h, radix, nfft, tmp3fft, out);
    if (out[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    } else {
        mp_copy(n, radix, tmp, out);
    }
}


void mp_squ(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int tmp[], 
        fft_int nfft, fft_float tmp1fft[], fft_float tmp2fft[])
{
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_csqu_nt_d1(fft_int nfft, fft_float d1[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
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
    /* ---- tmp = (upper) in * (lower) in ---- */
    mp_mul_i2d(n, radix, nfft, 0, in, tmp1fft);
    mp_mul_i2d(n, radix, nfft, shift, in, tmp2fft);
    mp_mul_cmul(nfft, tmp1fft, tmp2fft);
    mp_mul_d2i(n_h, radix, nfft, tmp2fft, tmp);
    /* ---- out = 2 * tmp + ((upper) in)^2 ---- */
    mp_mul_csqu_nt_d1(nfft, tmp1fft);
    mp_mul_d2i(n, radix, nfft, tmp1fft, out);
    if (tmp[0] != 0) {
        mp_add(n_h, radix, tmp, tmp, tmp);
        mp_add(n, radix, out, tmp, out);
    }
}


void mp_mulhf(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int tmp[], fft_int nfft, fft_float in1fft[], fft_float tmpfft[])
{
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_cmul_nt_d1(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
    dgt_int *in2dgt;
    
    in2dgt = (dgt_int *) &in2[2];
    shift = (nfft >> 1) + 1;
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
    /* ---- tmp = (upper) in1 * (upper) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in1, in1fft);
    mp_mul_i2d(n, radix, nfft, 0, in2, tmpfft);
    mp_mul_cmul(nfft, in1fft, tmpfft);
    mp_mul_d2i(n, radix, nfft, tmpfft, tmp);
    /* ---- out = tmp + (upper) in1 * (lower) in2 ---- */
    mp_mul_i2d(n, radix, nfft, shift, in2, tmpfft);
    mp_mul_cmul_nt_d1(nfft, in1fft, tmpfft);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, out);
    if (out[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    } else {
        mp_copy(n, radix, tmp, out);
    }
}


void mp_mulhf_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], fft_int in2[], 
        fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[])
{
    void mp_copy(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul_nt_d1(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
    dgt_int *in2dgt;
    
    in2dgt = (dgt_int *) &in2[2];
    shift = (nfft >> 1) + 1;
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
    /* ---- tmp = (upper) in1fft * (upper) in2 ---- */
    mp_mul_i2d(n, radix, nfft, 0, in2, tmpfft);
    mp_mul_cmul_nt_d1(nfft, in1fft, tmpfft);
    mp_mul_d2i(n, radix, nfft, tmpfft, tmp);
    /* ---- out = tmp + (upper) in1 * (lower) in2 ---- */
    mp_mul_i2d(n, radix, nfft, shift, in2, tmpfft);
    mp_mul_cmul_nt_d1(nfft, in1fft, tmpfft);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, out);
    if (out[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    } else {
        mp_copy(n, radix, tmp, out);
    }
}


void mp_squhf_use_infft(fft_int n, fft_int radix, fft_float infft[], fft_int in[], 
        fft_int out[], fft_int tmp[], fft_int nfft, fft_float tmpfft[])
{
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul_nt_d1(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_csqu_nt_d1(fft_int nfft, fft_float d1[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    fft_int n_h, shift;
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
    /* ---- tmp = (upper) infft * (lower) in ---- */
    mp_mul_i2d(n, radix, nfft, shift, in, tmpfft);
    mp_mul_cmul_nt_d1(nfft, infft, tmpfft);
    mp_mul_d2i(n_h, radix, nfft, tmpfft, tmp);
    /* ---- out = tmp + ((upper) infft)^2 ---- */
    mp_mul_csqu_nt_d1(nfft, infft);
    mp_mul_d2i(n, radix, nfft, infft, out);
    if (tmp[0] != 0) {
        mp_add(n, radix, out, tmp, out);
    }
}


void mp_mulh(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
        fft_int nfft, fft_float in1fft[], fft_float outfft[])
{
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in1, in1fft);
    mp_mul_i2d(n, radix, nfft, 0, in2, outfft);
    mp_mul_cmul(nfft, in1fft, outfft);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_mulh_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], 
        fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[])
{
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_cmul_nt_d1(fft_int nfft, fft_float d1[], fft_float d2[]);
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
    mp_mul_cmul_nt_d1(nfft, in1fft, outfft);
    mp_mul_d2i(n_h, radix, nfft, outfft, out);
}


void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, fft_float outfft[])
{
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_csqu(fft_int nfft, fft_float d1[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in, outfft);
    mp_mul_csqu(nfft, outfft);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_squh_save_infft(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int nfft, fft_float infft[], fft_float outfft[])
{
    void mp_mul_i2d(fft_int n, fft_int radix, fft_int nfft, fft_int shift, 
            fft_int in[], fft_float dout[]);
    void mp_mul_csqu_save_d1(fft_int nfft, fft_float d1[], fft_float d2[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_i2d(n, radix, nfft, 0, in, infft);
    mp_mul_csqu_save_d1(nfft, infft, outfft);
    mp_mul_d2i(n, radix, nfft, outfft, out);
}


void mp_squh_use_in1fft(fft_int n, fft_int radix, fft_float inoutfft[], fft_int out[], 
        fft_int nfft)
{
    void mp_mul_csqu_nt_d1(fft_int nfft, fft_float d1[]);
    void mp_mul_d2i(fft_int n, fft_int radix, fft_int nfft, fft_float din[], fft_int out[]);
    
    mp_mul_csqu_nt_d1(nfft, inoutfft);
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


void mp_mul_cmul(fft_int nfft, fft_float d1[], fft_float d2[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcmul(fft_int n, fft_float *a, fft_float *b);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d1[1]);
    cdft(nfft, 1, &d2[1]);
    d2[0] += d1[0];
    xr = d1[1] * d2[1] + d1[2] * d2[2];
    xi = d1[1] * d2[2] + d1[2] * d2[1];
    d2[1] = xr;
    d2[2] = xi;
    if (nfft > 2) {
        mp_mul_rcmul(nfft, &d1[1], &d2[1]);
    }
    d2[nfft + 1] *= d1[nfft + 1];
    cdft(nfft, -1, &d2[1]);
}


void mp_mul_cmul_nt_d1(fft_int nfft, fft_float d1[], fft_float d2[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcmul_nt_in1(fft_int n, fft_float *a, fft_float *b);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d2[1]);
    d2[0] += d1[0];
    xr = d1[1] * d2[1] + d1[2] * d2[2];
    xi = d1[1] * d2[2] + d1[2] * d2[1];
    d2[1] = xr;
    d2[2] = xi;
    if (nfft > 2) {
        mp_mul_rcmul_nt_in1(nfft, &d1[1], &d2[1]);
    }
    d2[nfft + 1] *= d1[nfft + 1];
    cdft(nfft, -1, &d2[1]);
}


void mp_mul_cmul_nt_d2(fft_int nfft, fft_float d1[], fft_float d2[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcmul_nt_in2(fft_int n, fft_float *a, fft_float *b);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d1[1]);
    d2[0] += d1[0];
    xr = d1[1] * d2[1] + d1[2] * d2[2];
    xi = d1[1] * d2[2] + d1[2] * d2[1];
    d2[1] = xr;
    d2[2] = xi;
    if (nfft > 2) {
        mp_mul_rcmul_nt_in2(nfft, &d1[1], &d2[1]);
    }
    d2[nfft + 1] *= d1[nfft + 1];
    cdft(nfft, -1, &d2[1]);
}


void mp_mul_cmul_nt_out(fft_int nfft, fft_float d1[], fft_float d2[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcmul_nt_out(fft_int n, fft_float *a, fft_float *b);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d1[1]);
    cdft(nfft, 1, &d2[1]);
    d2[0] += d1[0];
    xr = d1[1] * d2[1] + d1[2] * d2[2];
    xi = d1[1] * d2[2] + d1[2] * d2[1];
    d2[1] = xr;
    d2[2] = xi;
    if (nfft > 2) {
        mp_mul_rcmul_nt_out(nfft, &d1[1], &d2[1]);
    }
    d2[nfft + 1] *= d1[nfft + 1];
}


void mp_mul_cmul_nt_d1_add(fft_int nfft, fft_float d1[], fft_float d2[], 
        fft_float d3[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcmul_nt_in1_add(fft_int n, fft_float *a, fft_float *b, fft_float *badd);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d2[1]);
    xr = d1[1] * d2[1] + d1[2] * d2[2];
    xi = d1[1] * d2[2] + d1[2] * d2[1];
    d3[1] += xr;
    d3[2] += xi;
    if (nfft > 2) {
        mp_mul_rcmul_nt_in1_add(nfft, &d1[1], &d2[1], &d3[1]);
    }
    d3[nfft + 1] += d1[nfft + 1] * d2[nfft + 1];
    cdft(nfft, -1, &d3[1]);
}


void mp_mul_csqu(fft_int nfft, fft_float d1[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcsqu(fft_int n, fft_float *a);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d1[1]);
    d1[0] *= 2;
    xr = d1[1] * d1[1] + d1[2] * d1[2];
    xi = 2 * d1[1] * d1[2];
    d1[1] = xr;
    d1[2] = xi;
    if (nfft > 2) {
        mp_mul_rcsqu(nfft, &d1[1]);
    }
    d1[nfft + 1] *= d1[nfft + 1];
    cdft(nfft, -1, &d1[1]);
}


void mp_mul_csqu_save_d1(fft_int nfft, fft_float d1[], fft_float d2[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcsqu_save(fft_int n, fft_float *a, fft_float *b);
    fft_float xr, xi;
    
    cdft(nfft, 1, &d1[1]);
    d2[0] = 2 * d1[0];
    xr = d1[1] * d1[1] + d1[2] * d1[2];
    xi = 2 * d1[1] * d1[2];
    d2[1] = xr;
    d2[2] = xi;
    if (nfft > 2) {
        mp_mul_rcsqu_save(nfft, &d1[1], &d2[1]);
    }
    d2[nfft + 1] = d1[nfft + 1] * d1[nfft + 1];
    cdft(nfft, -1, &d2[1]);
}


void mp_mul_csqu_nt_d1(fft_int nfft, fft_float d1[])
{
    void cdft(fft_int n, fft_int isgn, fft_float *a);
    void mp_mul_rcsqu_nt_in(fft_int n, fft_float *a);
    fft_float xr, xi;
    
    d1[0] *= 2;
    xr = d1[1] * d1[1] + d1[2] * d1[2];
    xi = 2 * d1[1] * d1[2];
    d1[1] = xr;
    d1[2] = xi;
    if (nfft > 2) {
        mp_mul_rcsqu_nt_in(nfft, &d1[1]);
    }
    d1[nfft + 1] *= d1[nfft + 1];
    cdft(nfft, -1, &d1[1]);
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


/* -------- mp_mul child^2 routines (mix RFFT routines) -------- */


void mp_mul_rcmul(fft_int n, fft_float *a, fft_float *b)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, ajr, aji, akr, aki, bjr, bji, bkr, bki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    yr = b[i];
    yi = b[i + 1];
    b[i] = xr * yr - xi * yi;
    b[i + 1] = xr * yi + xi * yr;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data a[] into RFFT data ---- */
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            ajr = a[j] - yr;
            aji = a[j + 1] - yi;
            akr = a[k] + yr;
            aki = a[k + 1] - yi;
            a[j] = ajr;
            a[j + 1] = aji;
            a[k] = akr;
            a[k + 1] = aki;
            /* ---- transform CFFT data b[] into RFFT data ---- */
            xr = b[j] - b[k];
            xi = b[j + 1] + b[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = b[j] - yr;
            xi = b[j + 1] - yi;
            yr = b[k] + yr;
            yi = b[k + 1] - yi;
            /* ---- cmul ---- */
            bjr = ajr * xr - aji * xi;
            bji = ajr * xi + aji * xr;
            bkr = akr * yr - aki * yi;
            bki = akr * yi + aki * yr;
            /* ---- transform RFFT data bxx into CFFT data ---- */
            xr = bjr - bkr;
            xi = bji + bki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            b[j] = bjr - yr;
            b[j + 1] = bji - yi;
            b[k] = bkr + yr;
            b[k + 1] = bki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcmul_nt_in1(fft_int n, fft_float *a, fft_float *b)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, bjr, bji, bkr, bki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    yr = b[i];
    yi = b[i + 1];
    b[i] = xr * yr - xi * yi;
    b[i + 1] = xr * yi + xi * yr;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data b[] into RFFT data ---- */
            xr = b[j] - b[k];
            xi = b[j + 1] + b[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = b[j] - yr;
            xi = b[j + 1] - yi;
            yr = b[k] + yr;
            yi = b[k + 1] - yi;
            /* ---- cmul ---- */
            bjr = a[j] * xr - a[j + 1] * xi;
            bji = a[j] * xi + a[j + 1] * xr;
            bkr = a[k] * yr - a[k + 1] * yi;
            bki = a[k] * yi + a[k + 1] * yr;
            /* ---- transform RFFT data bxx into CFFT data ---- */
            xr = bjr - bkr;
            xi = bji + bki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            b[j] = bjr - yr;
            b[j + 1] = bji - yi;
            b[k] = bkr + yr;
            b[k + 1] = bki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcmul_nt_in2(fft_int n, fft_float *a, fft_float *b)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, bjr, bji, bkr, bki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    yr = b[i];
    yi = b[i + 1];
    b[i] = xr * yr - xi * yi;
    b[i + 1] = xr * yi + xi * yr;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data a[] into RFFT data ---- */
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = a[j] - yr;
            xi = a[j + 1] - yi;
            yr = a[k] + yr;
            yi = a[k + 1] - yi;
            a[j] = xr;
            a[j + 1] = xi;
            a[k] = yr;
            a[k + 1] = yi;
            /* ---- cmul ---- */
            bjr = b[j] * xr - b[j + 1] * xi;
            bji = b[j] * xi + b[j + 1] * xr;
            bkr = b[k] * yr - b[k + 1] * yi;
            bki = b[k] * yi + b[k + 1] * yr;
            /* ---- transform RFFT data bxx into CFFT data ---- */
            xr = bjr - bkr;
            xi = bji + bki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            b[j] = bjr - yr;
            b[j + 1] = bji - yi;
            b[k] = bkr + yr;
            b[k + 1] = bki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcmul_nt_out(fft_int n, fft_float *a, fft_float *b)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, ajr, aji, akr, aki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    yr = b[i];
    yi = b[i + 1];
    b[i] = xr * yr - xi * yi;
    b[i + 1] = xr * yi + xi * yr;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data a[] into RFFT data ---- */
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            ajr = a[j] - yr;
            aji = a[j + 1] - yi;
            akr = a[k] + yr;
            aki = a[k + 1] - yi;
            a[j] = ajr;
            a[j + 1] = aji;
            a[k] = akr;
            a[k + 1] = aki;
            /* ---- transform CFFT data b[] into RFFT data ---- */
            xr = b[j] - b[k];
            xi = b[j + 1] + b[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = b[j] - yr;
            xi = b[j + 1] - yi;
            yr = b[k] + yr;
            yi = b[k + 1] - yi;
            /* ---- cmul ---- */
            b[j] = ajr * xr - aji * xi;
            b[j + 1] = ajr * xi + aji * xr;
            b[k] = akr * yr - aki * yi;
            b[k + 1] = akr * yi + aki * yr;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcmul_nt_in1_add(fft_int n, fft_float *a, fft_float *b, fft_float *badd)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, bjr, bji, bkr, bki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    yr = b[i];
    yi = b[i + 1];
    badd[i] += xr * yr - xi * yi;
    badd[i + 1] += xr * yi + xi * yr;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data b[] into RFFT data ---- */
            xr = b[j] - b[k];
            xi = b[j + 1] + b[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = b[j] - yr;
            xi = b[j + 1] - yi;
            yr = b[k] + yr;
            yi = b[k + 1] - yi;
            /* ---- cmul + add ---- */
            bjr = badd[j] + (a[j] * xr - a[j + 1] * xi);
            bji = badd[j + 1] + (a[j] * xi + a[j + 1] * xr);
            bkr = badd[k] + (a[k] * yr - a[k + 1] * yi);
            bki = badd[k + 1] + (a[k] * yi + a[k + 1] * yr);
            /* ---- transform RFFT data bxx into CFFT data ---- */
            xr = bjr - bkr;
            xi = bji + bki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            badd[j] = bjr - yr;
            badd[j + 1] = bji - yi;
            badd[k] = bkr + yr;
            badd[k + 1] = bki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcsqu(fft_int n, fft_float *a)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, ajr, aji, akr, aki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    a[i] = xr * xr - xi * xi;
    a[i + 1] = 2 * xr * xi;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data a[] into RFFT data ---- */
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = a[j] - yr;
            xi = a[j + 1] - yi;
            yr = a[k] + yr;
            yi = a[k + 1] - yi;
            /* ---- csqu ---- */
            ajr = xr * xr - xi * xi;
            aji = 2 * xr * xi;
            akr = yr * yr - yi * yi;
            aki = 2 * yr * yi;
            /* ---- transform RFFT data axx into CFFT data ---- */
            xr = ajr - akr;
            xi = aji + aki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            a[j] = ajr - yr;
            a[j + 1] = aji - yi;
            a[k] = akr + yr;
            a[k + 1] = aki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcsqu_save(fft_int n, fft_float *a, fft_float *b)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, ajr, aji, akr, aki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    b[i] = xr * xr - xi * xi;
    b[i + 1] = 2 * xr * xi;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- transform CFFT data a[] into RFFT data ---- */
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            xr = a[j] - yr;
            xi = a[j + 1] - yi;
            yr = a[k] + yr;
            yi = a[k + 1] - yi;
            a[j] = xr;
            a[j + 1] = xi;
            a[k] = yr;
            a[k + 1] = yi;
            /* ---- csqu ---- */
            ajr = xr * xr - xi * xi;
            aji = 2 * xr * xi;
            akr = yr * yr - yi * yi;
            aki = 2 * yr * yi;
            /* ---- transform RFFT data axx into CFFT data ---- */
            xr = ajr - akr;
            xi = aji + aki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            b[j] = ajr - yr;
            b[j + 1] = aji - yi;
            b[k] = akr + yr;
            b[k + 1] = aki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


void mp_mul_rcsqu_nt_in(fft_int n, fft_float *a)
{
    fft_int i, i0, j, k;
    fft_float ec, w1r, w1i, wkr, wki, wdr, wdi, ss;
    fft_float xr, xi, yr, yi, ajr, aji, akr, aki;
    
    ec = 2 * FC_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = fft_cos(ec);
    wdr = fft_sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    xr = a[i];
    xi = a[i + 1];
    a[i] = xr * xr - xi * xi;
    a[i + 1] = 2 * xr * xi;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 2) {
            i0 = 2;
        }
        for (j = i - 2; j >= i0; j -= 2) {
            k = n - j;
            xr = wkr + ss * wdi;
            xi = wki + ss * (FC_HALF - wdr);
            wkr = wdr;
            wki = wdi;
            wdr = xr;
            wdi = xi;
            /* ---- csqu ---- */
            xr = a[j];
            xi = a[j + 1];
            yr = a[k];
            yi = a[k + 1];
            ajr = xr * xr - xi * xi;
            aji = 2 * xr * xi;
            akr = yr * yr - yi * yi;
            aki = 2 * yr * yi;
            /* ---- transform RFFT data axx into CFFT data ---- */
            xr = ajr - akr;
            xi = aji + aki;
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            a[j] = ajr - yr;
            a[j + 1] = aji - yi;
            a[k] = akr + yr;
            a[k + 1] = aki - yi;
        }
        if (i0 == 2) {
            break;
        }
        wkr = FC_HALF * fft_sin(ec * i0);
        wki = FC_HALF * fft_cos(ec * i0);
        wdr = FC_HALF - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = FC_HALF - wkr;
        i = i0;
    }
}


/* -------- mp_inv routines -------- */


fft_int mp_inv(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[])
{
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_inv_init(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    fft_int mp_inv_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmp1fft[], 
            fft_float tmp2fft[]);
    fft_int n_nwt, nfft_nwt, thr, prc;
    
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
                tmp1, tmp2, nfft_nwt, tmp1fft, tmp2fft);
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
    return 0;
}


fft_int mp_sqrt(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[])
{
    void mp_load_0(fft_int n, fft_int radix, fft_int out[]);
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_sqrt_init(fft_int n, fft_int radix, fft_int in[], fft_int out[], fft_int out_rev[]);
    fft_int mp_sqrt_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
            fft_int inout_rev[], fft_int tmp[], fft_int nfft, fft_float tmp1fft[], 
            fft_float tmp2fft[], fft_int *n_tmp1fft);
    fft_int n_nwt, nfft_nwt, thr, prc, n_tmp1fft;
    
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
    mp_sqrt_init(n_nwt, radix, in, out, tmp1);
    n_tmp1fft = 0;
    thr = 8;
    do {
        n_nwt = nfft_nwt + 2;
        if (n_nwt > n) {
            n_nwt = n;
        }
        prc = mp_sqrt_newton(n_nwt, radix, in, out, 
                tmp1, tmp2, nfft_nwt, tmp1fft, tmp2fft, 
                &n_tmp1fft);
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
    return 0;
}


fft_int mp_invisqrt(fft_int n, fft_int radix, fft_int in, fft_int out[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, 
        fft_float tmp1fft[], fft_float tmp2fft[])
{
    fft_int mp_get_nfft_init(fft_int radix, fft_int nfft_max);
    void mp_invisqrt_init(fft_int n, fft_int radix, fft_int in, fft_int out[]);
    fft_int mp_invisqrt_newton(fft_int n, fft_int radix, fft_int in, fft_int inout[], 
            fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmp1fft[], 
            fft_float tmp2fft[]);
    fft_int n_nwt, nfft_nwt, thr, prc;
    
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
                tmp1, tmp2, nfft_nwt, tmp1fft, tmp2fft);
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
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmp1fft[], 
        fft_float tmp2fft[])
{
    void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_mulh(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int nfft, fft_float in1fft[], fft_float outfft[]);
    void mp_mulh_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], 
            fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[]);
    fft_int n_h, shift, prc;
    
    shift = (nfft >> 1) + 1;
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp1 = inout * (upper) in (half to normal precision) ---- */
    mp_round(n, radix, shift, inout);
    mp_mulh(n, radix, inout, in, tmp1, 
            nfft, tmp1fft, tmp2fft);
    /* ---- tmp2 = 1 - tmp1 ---- */
    mp_load_1(n, radix, tmp2);
    mp_sub(n, radix, tmp2, tmp1, tmp2);
    /* ---- tmp2 -= inout * (lower) in (half precision) ---- */
    mp_mulh_use_in1fft(n, radix, tmp1fft, shift, in, tmp1, 
            nfft, tmp2fft);
    mp_sub(n_h, radix, tmp2, tmp1, tmp2);
    /* ---- get precision ---- */
    prc = -tmp2[1];
    if (tmp2[0] == 0) {
        prc = nfft + 1;
    }
    /* ---- tmp2 *= inout (half precision) ---- */
    mp_mulh_use_in1fft(n_h, radix, tmp1fft, 0, tmp2, tmp2, 
            nfft, tmp2fft);
    /* ---- inout += tmp2 ---- */
    if (tmp2[0] != 0) {
        mp_add(n, radix, inout, tmp2, inout);
    }
    return prc;
}


fft_int mp_sqrt_newton(fft_int n, fft_int radix, fft_int in[], fft_int inout[], 
        fft_int inout_rev[], fft_int tmp[], fft_int nfft, fft_float tmp1fft[], 
        fft_float tmp2fft[], fft_int *n_tmp1fft)
{
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_mulh(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[], 
            fft_int nfft, fft_float in1fft[], fft_float outfft[]);
    void mp_squh(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int nfft, fft_float outfft[]);
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
    /* ---- tmp = inout_rev^2 (1/4 to half precision) ---- */
    mp_round(n_h, radix, (nfft_h >> 1) + 1, inout_rev);
    if (*n_tmp1fft != nfft_h) {
        mp_squh(n_h, radix, inout_rev, tmp, 
                nfft_h, tmp1fft);
    } else {
        mp_squh_use_in1fft(n_h, radix, tmp1fft, tmp, nfft_h);
    }
    /* ---- tmp = inout_rev - inout * tmp (half precision) ---- */
    mp_round(n, radix, shift, inout);
    mp_mulh(n_h, radix, inout, tmp, tmp, 
            nfft, tmp1fft, tmp2fft);
    mp_sub(n_h, radix, inout_rev, tmp, tmp);
    /* ---- inout_rev += tmp ---- */
    mp_add(n_h, radix, inout_rev, tmp, inout_rev);
    /* ---- tmp = in - inout^2 (half to normal precision) ---- */
    mp_squh_use_in1fft(n, radix, tmp1fft, tmp, nfft);
    mp_sub(n, radix, in, tmp, tmp);
    /* ---- get precision ---- */
    prc = in[1] - tmp[1];
    if (((dgt_int *) &in[2])[0] > ((dgt_int *) &tmp[2])[0]) {
        prc++;
    }
    if (tmp[0] == 0) {
        prc = nfft + 1;
    }
    /* ---- tmp = tmp * inout_rev / 2 (half precision) ---- */
    mp_round(n_h, radix, shift, inout_rev);
    mp_mulh(n_h, radix, inout_rev, tmp, tmp, 
            nfft, tmp1fft, tmp2fft);
    *n_tmp1fft = nfft;
    mp_idiv_2(n_h, radix, tmp, tmp);
    /* ---- inout += tmp ---- */
    if (tmp[0] != 0) {
        mp_add(n, radix, inout, tmp, inout);
    }
    return prc;
}


fft_int mp_invisqrt_newton(fft_int n, fft_int radix, fft_int in, fft_int inout[], 
        fft_int tmp1[], fft_int tmp2[], fft_int nfft, fft_float tmp1fft[], 
        fft_float tmp2fft[])
{
    void mp_load_1(fft_int n, fft_int radix, fft_int out[]);
    void mp_round(fft_int n, fft_int radix, fft_int m, fft_int inout[]);
    void mp_add(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_sub(fft_int n, fft_int radix, fft_int in1[], fft_int in2[], fft_int out[]);
    void mp_imul(fft_int n, fft_int radix, fft_int in1[], fft_int in2, fft_int out[]);
    void mp_idiv_2(fft_int n, fft_int radix, fft_int in[], fft_int out[]);
    void mp_squh_save_infft(fft_int n, fft_int radix, fft_int in[], fft_int out[], 
            fft_int nfft, fft_float infft[], fft_float outfft[]);
    void mp_mulh_use_in1fft(fft_int n, fft_int radix, fft_float in1fft[], 
            fft_int shift, fft_int in2[], fft_int out[], fft_int nfft, fft_float outfft[]);
    fft_int n_h, shift, prc;
    
    shift = (nfft >> 1) + 1;
    n_h = n / 2 + 1;
    if (n_h < n - shift) {
        n_h = n - shift;
    }
    /* ---- tmp1 = in * inout^2 (half to normal precision) ---- */
    mp_round(n, radix, shift, inout);
    mp_squh_save_infft(n, radix, inout, tmp1, 
            nfft, tmp1fft, tmp2fft);
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
    mp_mulh_use_in1fft(n_h, radix, tmp1fft, 0, tmp2, tmp2, 
            nfft, tmp2fft);
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


