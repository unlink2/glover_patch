#include "include/render.h"
#include "include/memwatch.h"

#include "include/font8x8_basic.h"

void render() {
    memwatch *pmemwatch = memwatch_from_addr(MEMWATCH_STRUCT);
    render_memwatch(pmemwatch);

}

HWORD_T *get_frame_buffer() {
    HWORD_T *(*pfunction)();
    pfunction = (HWORD_T *(*)())__OS_GET_CURRENT_FRAMEBUFFER;
    HWORD_T *pfb = pfunction();

    return pfb;
}

void gputs(char *str, HWORD_T *pframebuffer, WORD_T x, WORD_T y, const WORD_T *pcharset, HWORD_T fg, HWORD_T bg) {
    while (str[0] != '\0') {
        draw_char(str[0], pframebuffer, x, y, pcharset, fg, bg);
        x += CHAR_W; // next
        str++; // next char
    }
}

inline void draw_char(char c, HWORD_T *pframebuffer, WORD_T x, WORD_T y, const WORD_T *pcharset, HWORD_T fg, HWORD_T bg) {
    // initial character offset
    const WORD_T *pchar = pcharset+(WORD_T)c*8;
    HWORD_T offset = SCREEN_W * y + x; // start address of framebuffer
    for (int y = 0; y < CHAR_H; y++) {
        HWORD_T fdata = pchar[0];
        for (int x = 0; x < CHAR_W; x++) {
            HWORD_T set = fdata & 1 << x;
            if (set) {
                __FB_WRITE_HW(pframebuffer, fg, offset);
            } else {
                __FB_WRITE_HW(pframebuffer, bg, offset);
            }
            offset++; // +1 offset
        }
        pchar++; // +1 font pixel
        offset += SCREEN_W-CHAR_W; // next line
    }
}

void write_to_framebuffer(HWORD_T *pframebuffer, HWORD_T color, WORD_T offset) {
    __FB_WRITE_HW(pframebuffer, color, offset);
}


