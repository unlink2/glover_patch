#include "include/rdp.h"
#include "include/memory.h"

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

int rdp_sync_load(WORD_T *pbuffer) {
    pbuffer[0] = 0x31000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_sync_tile(WORD_T *pbuffer) {
    pbuffer[0] = 0x28000000;
    pbuffer[1] = 0x00000000;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_test_texture(HWORD_T *pfont, WORD_T *pbuffer) {
    // set texture mode
    pbuffer[0] = 0x2F208000;
    pbuffer[1] = 0x00404040;
    pbuffer[2] = 0x3C0000C1;
    pbuffer[3] = 0x0F2001FF;

    // set texture image
    pbuffer[4] = 0x3D100007;
    pbuffer[5] = (WORD_T)pfont;

    // set tile
    pbuffer[6] = 0x35100400;
    pbuffer[7] = 0x00000000;

    // set texture rect
    pbuffer[8] = 0x34000000;
    pbuffer[9] = 0x0001C01C;
    pbuffer[10] = 0x24140110;
    pbuffer[11] = 0x001000D0;
    pbuffer[12] = 0x00000000;
    pbuffer[13] = 0x02000200;

    // sync
    pbuffer[14] = 0x28000000;
    pbuffer[15] = 0x00000000;

    rdp_send_dl(pbuffer, pbuffer+32);
    return 32;
}

int rdp_draw_primitives(WORD_T *pbuffer) {
    // enable primitive mode
    pbuffer[0] = 0xEFB000FF;
    pbuffer[1] = 0x00004004;
    rdp_send_dl(pbuffer, pbuffer+8);
    return 8;
}

int rdp_draw_rect(WORD_T color, int tx, int ty, int bx, int by, WORD_T *pbuffer) {
    // set color
    pbuffer[0] = 0xF7000000;
    pbuffer[1] = color;

    pbuffer[2] = 0xF6000000 | ( bx << 14 ) | ( by << 2 );
    pbuffer[3] = ( tx << 14 ) | ( ty << 2 );
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}



int rdp_load_char(char c, HWORD_T *pfont, WORD_T *pbuffer) {
    // set texture image
    pbuffer[0] = 0x3D080007; // load texture 16 bitdepth rgba, width-1
    pbuffer[1] = (WORD_T)pfont; // & 0x00FFFFFF; // pointer

    // set tile, set output
    pbuffer[2] = 0x35080000;
    pbuffer[3] = 0x0000C030; // no mirroring

    // load tile
    int sl = 0;
    int tl = 0;
    int sh = 8;
    int th = 8;
    pbuffer[4] =  0x34000000 | (((sl << 2) & 0xFFF) << 12) | ((tl << 2) & 0xFFF);
    pbuffer[5] = 0x00000000 | (((sh << 2) & 0xFFF) << 12) | ((th << 2) & 0xFFF);

    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_texture_mode(WORD_T *pbuffer) {
    // setup texture mode
    pbuffer[0] = 0x2FA000FF;
    pbuffer[1] = 0x00004001;
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}

int rdp_draw_char(WORD_T *pbuffer) {
    //float x_scale = 1.0;
    //float y_scale = 1.0;
    int xs = 1; //(int)((1.0 / x_scale) * 4096.0);
    int ys = 1; //(int)((1.0 / y_scale) * 1024.0);

    int bx = 58;
    int by = 58;
    int tx = 50;
    int ty = 50;
    int s = 0;
    int t = 0;
    int texslot = 0;

    pbuffer[0] =  0x24000000 | (bx << 14) | (by << 2);
    pbuffer[1] = ((texslot & 0x7) << 24) | (tx << 14) | (ty << 2);
    pbuffer[2] = (s << 16) | t;
    pbuffer[3] = (xs & 0xFFFF) << 16 | (ys & 0xFFFF);
    rdp_send_dl(pbuffer, pbuffer+8);

    return 8;
}
