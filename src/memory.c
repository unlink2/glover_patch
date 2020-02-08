#include "include/memory.h"
#include "include/utility.h"
#include "include/render.h"
#include "include/font8x8_basic.h"

void init_mem() {
    get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE, 1);
    *pframe_advance = 0x00;

    // draw intro message
    // unrolled draw loop to avoid ram usage during init
    HWORD_T *pfb = get_frame_buffer();
    draw_char('g', pfb, 10, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('i', pfb, 18, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('t', pfb, 26, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('h', pfb, 34, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('u', pfb, 42, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('b', pfb, 50, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('.', pfb, 58, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('c', pfb, 66, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('o', pfb, 74, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('m', pfb, 82, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('/', pfb, 90, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('u', pfb, 98, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('n', pfb, 106, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('l', pfb, 114, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('i', pfb, 122, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('n', pfb, 130, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('k', pfb, 138, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('2', pfb, 146, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('/', pfb, 154, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('g', pfb, 162, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('l', pfb, 170, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('o', pfb, 178, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('v', pfb, 186, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('e', pfb, 194, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('r', pfb, 202, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('_', pfb, 210, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('p', pfb, 218, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('a', pfb, 226, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('t', pfb, 234, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('c', pfb, 242, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
    draw_char('h', pfb, 250, 10, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
}
