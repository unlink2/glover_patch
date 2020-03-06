#include "include/debug.h"
#include "include/render.h"
#include "include/utility.h"

static volatile struct pi_regs* const pir = (struct pi_regs *)0xa4600000;

#ifndef __LP64__
// libdragon cache_op helper
#define cache_op(op) \
    addr=(void*)(((unsigned long)addr)&(~3));\
    for (;length>0;length-=4,addr+=4) \
        asm ("\tcache %0,(%1)\n"::"i" (op), "r" (addr))
#else
#define cache_op(op)
#endif


BOOLEAN ed_init_done = 0;

void data_cache_hit_writeback(volatile void *addr, unsigned long length) {
        cache_op(0x19);
}

void data_cache_hit_writeback_invalidate(volatile void * addr, unsigned long length) {
        cache_op(0x15);
}

BOOLEAN dma_busy() {
    return pir->status & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY);
}

void pi_read(void *ram, unsigned long pi_address, unsigned long len) {
    // void *(*pdisableint)() = (void *(*)())DISABLE_INT;
    // void *(*prestoreint)() = (void *(*)())RESTORE_INT;
    pi_address &= 0x1FFFFFFF;

    data_cache_hit_writeback_invalidate(ram, len);

    // pdisableint();

    while (dma_busy());
    IO_WRITE(PI_STATUS_REG, 3);
    pir->ram_address = ram;
    pir->pi_address = pi_address;
    pir->write_length = len-1;
    while (dma_busy());

    // prestoreint();
    /*BYTE_T *addr = (BYTE_T*)pi_address;
    BYTE_T *dest = (BYTE_T*)ram;
    for (int i = 0; i < len; i++) {
        dest[i] = addr[i];
    }*/
}

void pi_write(void *ram, unsigned long pi_address, unsigned long len) {
    // void *(*pdisableint)() = (void *(*)())DISABLE_INT;
    // void *(*prestoreint)() = (void *(*)())RESTORE_INT;
    pi_address &= 0x1FFFFFFF;

    data_cache_hit_writeback(ram, len);

    // pdisableint();

    while (dma_busy());
    IO_WRITE(PI_STATUS_REG, 3);
    pir->ram_address = ram;
    pir->pi_address = pi_address;
    pir->write_length = len-1;
    while (dma_busy());

    // prestoreint();
    /*BYTE_T *addr = (BYTE_T*)pi_address;
    BYTE_T *src = (BYTE_T*)ram;
    for (int i = 0; i < len; i++) {
        addr[i] = src[i];
    }*/
}

void evd_reg_write(u16 reg, u32 val) {
    pi_write(&val, ED_REGS(reg), 4);
}

u32 evd_reg_read(u16 reg) {
    u32 val;
    pi_read(&val, ED_REGS(reg), 4);
    return val;
}

void evd_init() {
    IO_WRITE(PI_BSD_DOM1_LAT_REG, 0x04);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, 0x0C);

    evd_reg_write(REG_KEY, 0xAA55);
    evd_reg_write(REG_SYS_CFG, 0);

    evd_usb_init();

    evd_set_save_type(SAVE_OFF);

    ed_init_done = TRUE;
}

void evd_usb_init() {
    u8 buff[512];
    u8 resp;
    evd_reg_write(REG_USB_CFG, USB_CMD_RD_NOP); // disable USB read/write

    // flush fifo buffer
    while (evd_usb_can_read()) {
        resp = evd_usb_read(buff, 512);
        if (resp) {
            break;
        }
    }
}

BOOLEAN evd_usb_can_read() {
    u32 status = evd_reg_read(REG_USB_CFG) & (USB_STA_PWR | USB_STA_RXF);
    if (status == USB_STA_PWR) {
        return 1;
    }
    return 0;
}

BOOLEAN evd_usb_can_write() {
    u32 status = evd_reg_read(REG_USB_CFG) & (USB_STA_PWR | USB_STA_TXE);
    if (status == USB_STA_PWR) {
        return 1;
    }
    return 0;
}

BOOLEAN evd_usb_read(void *dst, u32 len) {
    BOOLEAN resp = 0;
    u16 blen, baddr;

    while (len) {
        blen = 512;
        if (blen > len) {
            blen = len;
        }
        baddr = 512 - blen;

        evd_reg_write(REG_USB_CFG, USB_CMD_RD | baddr);
        resp = evd_usb_busy();
        if (resp) {
            break;
        }
        pi_read(dst, ED_REGS(REG_USB_DAT + baddr), blen);

        dst += blen;
        len -= blen;
    }

    return resp;
}

BOOLEAN evd_usb_write(void *src, u32 len) {
    BOOLEAN resp = 0;
    u16 blen, baddr;

    evd_reg_write(REG_USB_CFG, USB_CMD_WR_NOP);

    while (len) {
        blen = 512;
        if (blen > len) {
            blen = len;
        }
        baddr = 512 - blen;

        pi_write(src, ED_REGS(REG_USB_DAT + baddr), blen);
        src += 512;

        evd_reg_write(REG_USB_CFG, USB_CMD_WR | baddr);

        resp = evd_usb_busy();
        if (resp) {
            break;
        }

        len -= blen;
    }

    return resp;
}

BOOLEAN evd_usb_busy() {
    u32 tout = 0;
    while ((evd_reg_read(REG_USB_CFG) & USB_STA_ACT) != 0) {
        // delay
        if (tout++ != 8192) {
            continue;
        }
        evd_reg_write(REG_USB_CFG, USB_CMD_RD_NOP);
        return BI_ERR_USB_TOUT;
    }

    return 0;
}

void evd_set_save_type(u8 type) {
    evd_reg_write(REG_RAM_CFG, type);
}

void evd_echo_terminal() {
    if (ed_init_done != TRUE) {
        return;
    }

    u8 data[BI_MIN_USB_SIZE + 1];
    u8 tout;

    if (!evd_usb_can_read()) {
        return;
    }

    tout = evd_usb_read(data, BI_MIN_USB_SIZE); // read from serial
    if (tout) {
        return;
    }

    evd_usb_write(data, BI_MIN_USB_SIZE); // send back
}
