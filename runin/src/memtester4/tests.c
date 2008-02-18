/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <memtest@discworld.dyndns.org>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004 Charles Cazabon <memtest@discworld.dyndns.org>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the functions for the actual tests, called from the
 * main routine in memtester.c.  See other comments in that file.
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "types.h"
#include "sizes.h"

char progress[] = "-\\|/";
#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500

/* Function definitions. */
/* Takes aligned buffers.  Finds an arbitrary location in memory of a
 * user process (must be passed as an array of ints) and translates
 * it to physical memory location.  All this without kernel support
 * (besides /proc/kcore). Optionally specify a character alignment in
 * case the memory buffer is not byte-aligned. */
void kmemscan (ulv *nbuf, int block_size, int offset) {
	int kmem_file;
	int d;
    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) 
    	pagesize == 8192;    
	/* window manipulation, iterator, read retval, etc */
	int low, high, foo;
      	int rd;

	/* We will use the fact that Linux does not suballocate pages (does
	   any real OS do that?) to optimize the search by only looking in the
	   parts of the page which matter. */
	int fail_page_offset;
	int curr_page_offset;

	/* Physical address pointers */
	unsigned long phys_addr=0;
	unsigned long failure;
	unsigned long pages=0;

	/* Don't put weird values here (size < 0, start > 0) */
	int windowsize=10;
	int windowstart=-5;


	/* Now compute the offset (in chars) of the error from the page
	   boundary. */
	fail_page_offset = ((int) (&nbuf[offset])) %pagesize;

	kmem_file = open("/proc/kcore",0);
	if (kmem_file < 0) {
		printf("Unable to open /proc/kcore.  No memory failure scanning possible.\n");
		return;
	}

	/* Set up window */
	if ((low = offset + windowstart) < 0) low=0;
	if ((high = low + windowsize) > block_size) high=block_size;
	foo=low;

	/* fail_page_offset is now the offset of the beginning of the
	 * window.
	 */
	fail_page_offset -= ((offset - low) * sizeof(int));
	if (fail_page_offset < 0) fail_page_offset+=memtester_pagesize();

	printf("%d %x fail_page_offset\n",fail_page_offset,fail_page_offset);

	fprintf(stderr, "Scanning /proc/kcore.  Fire in the hole!\n");


	/* Start by seeking to the start of the area on the page where
	 * we'st be lookin'
	 */
	lseek(kmem_file,pages*pagesize+fail_page_offset,SEEK_SET);
	phys_addr=pages*pagesize+fail_page_offset;

	/* We now use lseeks to (hugely) improve the performance of this
	   thing.  Large memory systems were extremely painful before. 
	   Away we go! */
	while( (rd=read(kmem_file, &d, sizeof(int))) != 0) {
		phys_addr += rd;
		if(nbuf[foo] == d) {
			++foo;
		} else {
			foo = low;	
			/* Every time we miss, skip to the next page. */
			++pages;
			lseek(kmem_file,pages*pagesize+fail_page_offset,SEEK_SET);
			phys_addr=pages*pagesize+fail_page_offset;
			continue;
		}
		/* If foo made it to high, we've found it. */
		if(foo==high) {
			/* Convert to address.  Our physical address is
			   pointing at high, we need it to point at the
			   window center.  */
			failure = phys_addr - 
				sizeof(int) * (windowsize + windowstart);
			fprintf(stderr, "Possible location of memory failure: %p (%dM) on page %d\n",
				(void *) failure,
				(int) (failure/1024/1024),
				(int) (failure/pagesize));
			close(kmem_file);
			return;
		} 

	}
	close(kmem_file);
	fprintf(stderr, "The memory failure location could not be determined. This,\nwhile not provably impossible, should never happen under practical\ncircumstances unless there is a bug or the memtst program image is\n corrupt.\n");
}

int compare_regions(ulv *bufa, ulv *bufb, size_t count) {
    int r = 0;
    size_t i;
    ulv *p1 = bufa;
    ulv *p2 = bufb;

    for (i = 0; i < count; i++, p1++, p2++) {
        if (*p1 != *p2) {
            fprintf(stderr, "FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n", 
                (ul) *p1, (ul) *p2, (ul) i);
		kmemscan((ulv *)bufa, count, i);
            /* printf("Skipping to next test..."); */
            r = -1;
        }
    }
	kmemscan((ulv *)bufa, count, 0);
	kmemscan((ulv *)bufb, count, 0);
//	kmemscan((ulv *)bufa, count, count-1);
    return r;
}

int test_stuck_address(ulv *bufa, size_t count) {
    ulv *p1 = bufa;
    unsigned int j;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < 16; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            *p1 = ((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1);
            *p1++;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        for (i = 0; i < count; i++, p1++) {
            if (*p1 != (((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1))) {
                fprintf(stderr, 
                    "FAILURE: possible bad address line at offset 0x%08lx.\n", 
                    (ul) i);
                printf("Skipping to next test...\n");
                fflush(stdout);
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_random_value(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    ul j = 0;
    size_t i;

	putchar(' ');
    fflush(stdout);
    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = rand_ul();
		if (!(i % PROGRESSOFTEN)) {
			putchar('\b');
			putchar(progress[++j % PROGRESSLEN]);
            fflush(stdout);
		}
    }
	printf("\b \b");
    fflush(stdout);
    return compare_regions(bufa, bufb, count);
}

int test_xor_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ ^= q;
        *p2++ ^= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_sub_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ -= q;
        *p2++ -= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_mul_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ *= q;
        *p2++ *= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_div_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        if (!q) {
            q++;
        }
        *p1++ /= q;
        *p2++ /= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_or_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ |= q;
        *p2++ |= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_and_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ &= q;
        *p2++ &= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_seqinc_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = (i + q);
    }
    return compare_regions(bufa, bufb, count);
}

int test_solidbits_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < 64; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? UL_ONEBITS : 0;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_checkerboard_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < 64; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_blockseq_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < 256; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (ul) UL_BYTE(j);
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_walkbits0_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < 64; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = 0x00000001 << j;
            } else { /* Walk it back down. */
                *p1++ = *p2++ = 0x00000001 << (UL_LEN * 2 - j - 1);
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_walkbits1_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < UL_LEN * 2; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = UL_ONEBITS ^ (0x00000001 << j);
            } else { /* Walk it back down. */
                *p1++ = *p2++ = UL_ONEBITS ^ (0x00000001 << (UL_LEN * 2 - j - 1));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_bitspread_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (j = 0; j < UL_LEN * 2; j++) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (0x00000001 << j) | (0x00000001 << (j + 2))
                    : UL_ONEBITS ^ ((0x00000001 << j)
                                    | (0x00000001 << (j + 2)));
            } else { /* Walk it back down. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (0x00000001 << (UL_LEN * 2 - 1 - j)) | (0x00000001 << (UL_LEN * 2 + 1 - j))
                    : UL_ONEBITS ^ (0x00000001 << (UL_LEN * 2 - 1 - j)
                                    | (0x00000001 << (UL_LEN * 2 + 1 - j)));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

int test_bitflip_comparison(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j, k;
    ul q;
    size_t i;

	printf("           ");
    fflush(stdout);
    for (k = 0; k < UL_LEN; k++) {
        q = 0x00000001 << k;
        for (j = 0; j < 8; j++) {
    	    printf("\b\b\b\b\b\b\b\b\b\b\b");
            q = ~q;
            printf("setting %3u", k * 8 + j);
            fflush(stdout);
            p1 = (ulv *) bufa;
            p2 = (ulv *) bufb;
            for (i = 0; i < count; i++) {
                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
            }
           printf("\b\b\b\b\b\b\b\b\b\b\b");
            printf("testing %3u", k * 8 + j);
            fflush(stdout);
            if (compare_regions(bufa, bufb, count)) {
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}
