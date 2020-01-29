// c implemnentation of decompress.py
// based on https://github.com/jombo23/N64-Tools


int dec_fla2(unsigned char *pdata, int *compressed_size, int dec_s, unsigned char *poutput) {
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
    (*compressed_size) = inputs_position;

    return output_position;
}
