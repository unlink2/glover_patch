#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/fla2.h"



int dec_all(unsigned char *pdata, int offset, int datalen, unsigned char *poutput, char *poutdir) {
    for (int i = offset; i < datalen; i += 0x10) {
        // check if file starts with FLA2
        if (i+8 < datalen && strncmp("FLA2", (const char*)pdata+i, 4) == 0) {
            int csize = 0;
            int poutput_pos = dec_fla2(pdata+i+8, &csize, 0, poutput);

            char poutpath[512]; // should be large enough for regular use

            sprintf(poutpath, "%s%X.bin", poutdir, i);

            printf("%s %X %X\n", poutpath, poutput_pos, csize);
            // write poutput
            FILE *pofile = fopen(poutpath, "wb");
            for (int i = 0; i < poutput_pos; i++) {
                fputc(poutput[i], pofile);
            }
            fclose(pofile);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: decompress <rom file> <offset/all> <poutput file/directory>\n");
        return 1;
    }

    char *pinpath = argv[1];
    int offset = (int)strtol(argv[2], NULL, 16);
    char *poutpath = argv[3];

    FILE *pifile = fopen(pinpath, "rb");
    fseek(pifile, 0, SEEK_END);
    long flen = ftell(pifile);
    unsigned char *pdata = malloc(flen*sizeof(char));
    rewind(pifile);
    fread(pdata, flen, 1, pifile);
    fclose(pifile);

    unsigned char *poutput = malloc(0x1000000 * sizeof(char));
    if (strcmp("all", argv[2]) == 0) {
        int ret = dec_all(pdata, 0, flen, poutput, poutpath);
        free(poutput);
        free(pdata);
        return ret;
    }

    int csize = 0;
    int poutput_pos = dec_fla2(pdata+offset, &csize, 0, poutput);

	printf("%X %X\n", poutput_pos, csize);

    // write poutput
    FILE *pofile = fopen(poutpath, "wb");
    for (int i = 0; i < poutput_pos; i++) {
        fputc(poutput[i], pofile);
    }
    fclose(pofile);

    free(poutput);
    free(pdata);
    return 0;
}
