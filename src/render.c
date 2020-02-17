#include "include/render.h"
#include "include/memwatch.h"
#include "include/utility.h"
#include "include/font8x8_basic.h"
#include "include/debug.h"

void render() {
    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    // check if font is decompressed
    // test if pixel matches bg color
    // first char is NULL and therefore empty
    if (pfont[0] != 0xFFFF) {
        decompress_font((WORD_T*)font8x8_basic, pfont, 0x000F, 0xFFFF);
    }

    render_memwatch(&pmemwatch);

    // get_ptr(WORD_T, pbuffer, RDP_DL_BUFFER, 0xFF);
    // pbuffer += rdp_sync_pipe(pbuffer);
    // pbuffer += rdp_draw_rect(0xF00Ff00f, 0, 0, 30, 30, pbuffer);
    // pbuffer += rdp_draw_rect(0xFFFF00FF, 15, 15, 35, 35, pbuffer);
    // testing testing
    /*pbuffer += rdp_texture_mode(pbuffer);
    pbuffer += rdp_load_char('a', (HWORD_T*)0x80026DB0, pbuffer);
    pbuffer += rdp_draw_char(pbuffer);*/
}


void rdp_send_dl(WORD_T *start, WORD_T *end) {
    void *(*pdisableint)() = (void *(*)())DISABLE_INT;
    void *(*prestoreint)() = (void *(*)())RESTORE_INT;

    // wait for rdp to not be busy
    RDP_WAIT_PIPE(0x00);

    // turn off interrupts to prevent conflicts
    pdisableint();

    // set flush flag
    ((WORD_T *)0xA4100000)[3] = 0x15;

    // wait again to flush
    RDP_WAIT_PIPE(0x00);

    // & 0x00FFFFFF -> converts to physical address for dma
    ((volatile WORD_T*)0xA4100000)[0] = ((WORD_T)start) & 0x00FFFFFF;
    ((volatile WORD_T*)0xA4100000)[1] = ((WORD_T)end) & 0x00FFFFFF;
    prestoreint();
}

int rdp_sync_full(WORD_T *pbuffer) {
    // full pipeline sync
    pbuffer[0] = 0x29000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    // set frame buffer
    // pbuffer += 8;
    // pbuffer[0] = 0x3F20013F;
    // pbuffer[1] = (WORD_T)get_frame_buffer();
    // rdp_send_dl(pbuffer, pbuffer+8);
    return 8;
}

int rdp_sync_pipe(WORD_T *pbuffer) {
    pbuffer[0] = 0x27000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_draw_rect(WORD_T color, int tx, int ty, int bx, int by, WORD_T *pbuffer) {
    // enable primitive mode
    pbuffer[0] = 0xEFB000FF;
    pbuffer[1] = 0x00004004;
    // set color
    pbuffer[2] = 0xF7000000;
    pbuffer[3] = color;

    pbuffer[4] = 0xF6000000 | ( bx << 14 ) | ( by << 2 );
    pbuffer[5] = ( tx << 14 ) | ( ty << 2 );
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}



int rdp_load_char(char c, HWORD_T *pfont, WORD_T *pbuffer) {
    pbuffer[0] = 0xFD100000; // load texture 2 bitdepth
    pbuffer[1] = (WORD_T)pfont; // pointer

    // +4 because DMA needs to be aligned
    // copy sprite data
    pbuffer[2] = 0x35100000;
    pbuffer[3] = 0x07000000; // no mirrorning

    int sl = 8;
    int tl = 0;
    int sh = 8;
    int th = 0;
    pbuffer[4] =  0xF4000000 | (((sl << 2) & 0xFFF) << 12) | ((tl << 2) & 0xFFF);
    pbuffer[5] = (((sh << 2) & 0xFFF) << 12) | ((th << 2) & 0xFFF);

    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_texture_mode(WORD_T *pbuffer) {
    pbuffer += 32;
    // setup texture mode
    pbuffer[0] = 0xEFA000FF;
    pbuffer[1] = 0x00004001;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_draw_char(WORD_T *pbuffer) {
    //float x_scale = 1.0;
    //float y_scale = 1.0;
    int xs = 10; //(int)((1.0 / x_scale) * 4096.0);
    int ys = 10; //(int)((1.0 / y_scale) * 1024.0);

    int bx = 58;
    int by = 58;
    int tx = 50;
    int ty = 50;
    int s = 0;
    int t = 0;
    int texslot = 7;

    pbuffer[0] =  0xE4000000 | (bx << 14) | (by << 2);
    pbuffer[1] = ((texslot & 0x7) << 24) | (tx << 14) | (ty << 2);
    pbuffer[2] = (s << 16) | t;
    pbuffer[3] = (xs & 0xFFFF) << 16 | (ys & 0xFFFF);
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

HWORD_T *get_frame_buffer() {
    HWORD_T *(*pfunction)();
    pfunction = (HWORD_T *(*)())__OS_GET_CURRENT_FRAMEBUFFER;
    HWORD_T *pfb = pfunction();

    /*if ((HWORD_T)pfb == 0x80092030) {
        return (HWORD_T*)0x800B7830;
    } else {
        return (HWORD_T*)0x80092030;
    }*/
    return (HWORD_T*)(((WORD_T)pfb & 0x0FFFFFFF) | 0xA0000000);
}

void puts_xy(char *str, unsigned int x, unsigned int y) {
    void (*pfunction)(WORD_T, char*, WORD_T);
    pfunction = (void (*)(WORD_T, char*, WORD_T))PUTS_XY;

    // set x and y value
    WORD_T *px = PUTS_X_POS;
    WORD_T *py = PUTS_Y_POS;
    *px = x;
    *py = y;

    // TODO find out what arg1 and arg2 are for
    // need to make sure that
    // the call happens exactly in this order
    // with a0 == arg0, a1 == str, a2 == arg2
    // arg2 might be a ptr to a struct used for printing
    pfunction(0x80202240, str, 0x801ED324);
}

void decompress_font(WORD_T *pcharset, HWORD_T *pdest, HWORD_T fg, HWORD_T bg) {
    // this is rather slow, but it only has to be done once anyway
    for (int c = 0; c < 128; c++) {
        for (int y = 0; y < CHAR_H; y++) {
            WORD_T fdata = pcharset[0];
            for (int x = 0; x < CHAR_H; x++) {
                HWORD_T set = fdata & 1 << x;
                if (set) {
                    *pdest = fg;
                } else {
                    *pdest = bg;
                }
                pdest++; // next hword
            }
            pcharset++; // next 8 pixels
        }
    }
}

void gputs(char *str, HWORD_T *pframebuffer, WORD_T x, WORD_T y, const WORD_T *pcharset, HWORD_T fg, HWORD_T bg) {
    while (str[0] != '\0') {
        INLINE_DRAW_CHAR(str[0], pframebuffer, x, y, pcharset, fg, bg);
        x += CHAR_W; // next
        str++; // next char
    }
}

void draw_char(char c, HWORD_T *pframebuffer, WORD_T x, WORD_T y, const WORD_T *pcharset, HWORD_T fg, HWORD_T bg) {
    INLINE_DRAW_CHAR(c, pframebuffer, x, y, pcharset, fg, bg);
}

void gputsf(char *str, HWORD_T *pframebuffer, WORD_T x, WORD_T y, HWORD_T *pfont) {
    while (str[0] != '\0') {
        INLINE_DRAW_CHARF(str[0], pframebuffer, x, y, pfont);
        x += CHAR_W; // next
        str++; // next char
    }
}

void draw_charf(char c, HWORD_T *pframebuffer, WORD_T x, WORD_T y, HWORD_T *pfont) {
    INLINE_DRAW_CHARF(c, pframebuffer, x, y, pfont);
}

void write_to_framebuffer(HWORD_T *pframebuffer, HWORD_T color, WORD_T offset) {
    FB_WRITE_HW(pframebuffer, color, offset);
}


