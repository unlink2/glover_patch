#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// c implemnentation of decompress.py
// based on https://github.com/jombo23/N64-Tools


int decFLA2(unsigned char* pdata, int *compressed_size, int dec_s, unsigned char* poutput) {
    if (dec_s == 0) {
        dec_s = 0x1000000;
    }

    // set up a ring of 0x1000, init to 0; buffer pos = 0
    unsigned char ring[0x1000];
    for (int x = 0; x < 0x1000; x++) {
        ring[x] = 0;
    }

    unsigned long cmd = 0x10000;
    unsigned long cur = 0;

    int inputs_position = 0;
    int output_position = 0;

    while (output_position < dec_s) {
        if (cmd>0xFFFF) {
            cmd = pdata[inputs_position++] | 0x100;
        }
        cmd<<=1;
        if (cmd&0x100) {
            // if set, copy from ring to poutput
            unsigned char c = pdata[inputs_position++];
            // if zero, this is EOF
            if (!c) {
                break;
            }
            unsigned long b = (c << 4) & 0xF00;
            b |= pdata[inputs_position++];
            c &= 0xF;
            for (int i = 0; i < (c + 2); i++) {
                unsigned char v = ring[(cur - b) & 0xFFF];
                poutput[output_position++] = v;
                ring[cur] = v;
                cur += 1;
                cur &= 0xFFF;
            }
        }
        else {
            // if not set, flush next byte to poutput
            unsigned char v = pdata[inputs_position++];
            poutput[output_position++] = v;
            ring[cur] = v;
            cur += 1;
            cur &= 0xFFF;
        }
    }
    *compressed_size = inputs_position;

    return output_position;
}

int dec_all(unsigned char *pdata, int offset, int datalen, unsigned char *poutput, char *poutdir) {
    for (int i = offset; i < datalen; i += 0x10) {
        // check if file starts with FLA2
        if (i+8 < datalen && strncmp("FLA2", (const char*)pdata+i, 4) == 0) {
            int csize = 0;
            int poutput_pos = decFLA2(pdata+i+8, &csize, 0, poutput);

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
        return dec_all(pdata, 0, flen, poutput, poutpath);
    }

    int csize = 0;
    int poutput_pos = decFLA2(pdata+offset, &csize, 0, poutput);

	printf("%X %X\n", poutput_pos, csize);

    // write poutput
    FILE *pofile = fopen(poutpath, "wb");
    for (int i = 0; i < poutput_pos; i++) {
        fputc(poutput[i], pofile);
    }
    fclose(pofile);
    return 0;
}
