#include "include/rdp.h"
#include "include/memory.h"

WORD_T *get_pbuffer() {
    get_ptr(WORD_T, pbufferptr, RDP_DL_BUFFER_PTR, 1);
    return (WORD_T*)*pbufferptr;
}

void inc_pbuffer(u32 size) {
    get_ptr(WORD_T, pbufferptr, RDP_DL_BUFFER_PTR, 1);
    *pbufferptr += size * sizeof(WORD_T);
}

void set_pbuffer(WORD_T *ptr) {
    get_ptr(WORD_T, pbufferptr, RDP_DL_BUFFER_PTR, 1);
    *pbufferptr = (WORD_T)ptr;
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
    RDP_CLEAR_BUFFER(pbuffer, 8);

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
    RDP_CLEAR_BUFFER(pbuffer, 8);
    pbuffer[0] = 0x27000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_sync_load(WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);
    pbuffer[0] = 0x31000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_sync_tile(WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);
    pbuffer[0] = 0x28000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}


int rdp_draw_primitives(WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);
    // enable primitive mode
    pbuffer[0] = 0xEFB000FF;
    pbuffer[1] = 0x00004004;
    rdp_send_dl(pbuffer, pbuffer+8);
    return 8;
}

int rdp_draw_rect(WORD_T color, int tx, int ty, int bx, int by, WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);
    // set color
    pbuffer[0] = 0xF7000000;
    pbuffer[1] = color;

    pbuffer[2] = 0xF6000000 | ( bx << 14 ) | ( by << 2 );
    pbuffer[3] = ( tx << 14 ) | ( ty << 2 );
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_texture_mode(WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);
    // set texture mode
    pbuffer[0] = 0x2F002830;
    pbuffer[1] = 0x00404040;
    pbuffer[2] = 0x3C0000C1;
    pbuffer[3] = 0x0F2001FF;

    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_load_tile(HWORD_T *pfont, WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);

    // sync
    pbuffer[0] = 0x27000000;
    pbuffer[1] = 0x00000000;

    // set texture image
    pbuffer[2] = 0x3D100007;
    pbuffer[3] = (WORD_T)pfont;

    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_draw_tile(int xh, int yh, int w, int h, WORD_T *pbuffer) {
    RDP_CLEAR_BUFFER(pbuffer, 8);

    // sync
    // pbuffer[0] = 0x31000000;
    // pbuffer[1] = 0x00000000;

    // set tile
    pbuffer[0] = 0x35100400; //  0<<2,0<<2, 0, 7<<2,7<<2, sl 0.0 tl 0.0, sh 7.0 th 7.0
    pbuffer[1] = 0x00000000;

    // set texture rect
    pbuffer[2] = 0x34000000;
    pbuffer[3] = 0x0001C01C;

    // sync
    // pbuffer[6] = 0x28000000;
    // pbuffer[7] = 0x00000000;

    // draw texture rect
    pbuffer[4] = 0x24000000  | (xh+w) << 14 | (yh+h) << 2; // xh+w == xl; yh+h == yl
    pbuffer[5] = 0x00000000 | xh << 14 | yh << 2;
    pbuffer[6] = 0x00000000; // S 0.0 T 0.0
    pbuffer[7] = 0x04000400; // scale factor 1:1

    rdp_send_dl(pbuffer, pbuffer+8);

    return 16;
}
