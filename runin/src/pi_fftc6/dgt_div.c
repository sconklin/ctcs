#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DGT_PACK 10
#define DGT_PACK_LINE 5
#define DGT_LINE_BLOCK 20

main()
{
    FILE *fin, *fout;
    int c, j, k, l;
    char f_name[256];
    
    printf("data file converter: *.dat => *.txt\ninput data file?\n");
    scanf("%s", f_name);
    fin = fopen(f_name, "r");
    if (fin == NULL) {
        printf("no input file : %s\n", f_name);
        exit(1);
    }
    printf("output txt file?\n");
    scanf("%s", f_name);
    fout = fopen(f_name, "r");
    if (fout != NULL) {
        printf("error : %s exists\n", f_name);
        exit(1);
    }
    fout = fopen(f_name, "w");
    j = 0;
    k = 0;
    l = 0;
    while ((c = getc(fin)) != EOF) {
        if (isdigit(c) == 0) {
            if (isalpha(c) != 0) {
                putc('\n', fout);
                putc('\n', fout);
            }
            putc(c, fout);
            putc('\n', fout);
            putc('\n', fout);
            j = 0;
            k = 0;
            l = 0;
            continue;
        }
        putc(c, fout);
        if (++j >= DGT_PACK) {
            j = 0;
            if (++k >= DGT_PACK_LINE) {
                k = 0;
                putc('\n', fout);
                if (++l >= DGT_LINE_BLOCK) {
                    l = 0;
                    putc('\n', fout);
                }
            } else {
                putc(' ', fout);
            }
        }
        
    }
    fclose(fout);
    fclose(fin);
}

