#include "include/render.h"
#include "include/memwatch.h"

#include "include/font8x8_basic.h"

void render() {
    // check if font is decompressed
    get_ptr(HWORD_T, pfont, FONT8X8, 0x400);
    // test if pixel matches bg color
    // first char is NULL and therefore empty
    if (pfont[0] != 0xFFFF) {
        decompress_font((WORD_T*)font8x8_basic, pfont, 0x000F, 0xFFFF);
    }

    memwatch *pmemwatch = memwatch_from_addr(MEMWATCH_STRUCT);
    render_memwatch(pmemwatch);
}

HWORD_T *get_frame_buffer() {
    HWORD_T *(*pfunction)();
    pfunction = (HWORD_T *(*)())__OS_GET_CURRENT_FRAMEBUFFER;
    HWORD_T *pfb = pfunction();

    return pfb;
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

void gputs_dma(char *str, HWORD_T *pframebuffer, WORD_T x, WORD_T y, HWORD_T *pfont) {
    while (str[0] != '\0') {
        INLINE_DRAW_CHAR_DMA(str[0], pframebuffer, x, y, pfont);
        x += CHAR_W; // next
        str++; // next char
    }
}

void draw_char_dma(char c, HWORD_T *pframebuffer, WORD_T x, WORD_T y, HWORD_T *pfont) {
    INLINE_DRAW_CHAR_DMA(c, pframebuffer, x, y, pfont);
}

void write_to_framebuffer(HWORD_T *pframebuffer, HWORD_T color, WORD_T offset) {
    FB_WRITE_HW(pframebuffer, color, offset);
}


