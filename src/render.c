#include "include/render.h"
#include "include/memwatch.h"
#include "include/utility.h"
#include "include/font8x8_basic.h"
#include "include/debug.h"
#include "include/rdp.h"
#include "include/menu.h"
#include "include/matrix.h"

void render() {
    // reset dl ptr
    get_ptr(WORD_T, pbuffer, RDP_DL_BUFFER, RDP_DL_SIZE);
    set_pbuffer(pbuffer); // TODO investiage: add +8 removes weird rendering issue in menu string "Save Position"

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    // check if font is decompressed
    // test if pixel matches bg color
    // first char is NULL and therefore empty
    if (pfont[0] != 0xFFFF) {
        decompress_font((WORD_T*)font8x8_basic, pfont, 0x000F, 0xFFFF);
        // clear rdp buffer once as well
        get_ptr(WORD_T, pbuffer, RDP_DL_BUFFER, RDP_DL_SIZE);
        gmemset((BYTE_T*)pbuffer, 0x00, RDP_DL_SIZE*sizeof(WORD_T));
    }

    render_memwatch(&pmemwatch);
    render_menu(&pmenu);

    // TODO send all rdp commands at once
    // WORD_T *pend = get_pbuffer();
    // rdp_send_dl(pbuffer, pend);
}



HWORD_T *get_frame_buffer() {
    HWORD_T *(*pfunction)();
    pfunction = (HWORD_T *(*)())__OS_GET_CURRENT_FRAMEBUFFER;
    HWORD_T *pfb = pfunction();

    if ((WORD_T)pfb == 0x80092030) {
        return (HWORD_T*)0x800B7830;
    } else {
        return (HWORD_T*)0x80092030;
    }
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

void gputsrdp(char *str, WORD_T x, WORD_T y, HWORD_T *pfont) {
    WORD_T *pbuffer = get_pbuffer();
    inc_pbuffer(rdp_texture_mode(pbuffer));
    while (str[0] != '\0') {
        draw_charrdp(str[0], x, y, pfont);
        x += CHAR_W; // next
        str++; // next char
    }
}

void draw_charrdp(char c, WORD_T x, WORD_T y, HWORD_T *pfont) {
    HWORD_T *pchar = pfont+(WORD_T)c*CHAR_W*CHAR_H;
    WORD_T *pbuffer = get_pbuffer();
    inc_pbuffer(rdp_load_tile(pchar, pbuffer));
    pbuffer = get_pbuffer();
    inc_pbuffer(rdp_draw_tile(x, y, 8, 8, pbuffer));
}

void write_to_framebuffer(HWORD_T *pframebuffer, HWORD_T color, WORD_T offset) {
    FB_WRITE_HW(pframebuffer, color, offset);
}

void draw_rect(u32 x, u32 y, u32 w, u32 h, u32 color) {
    WORD_T *pbuffer = get_pbuffer();
    inc_pbuffer(rdp_draw_primitives(pbuffer));
    inc_pbuffer(rdp_draw_rect(color, x, y, x+w, y+h, pbuffer));
}
