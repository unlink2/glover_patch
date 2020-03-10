#include "include/debug.h"
#include "include/render.h"
#include "include/utility.h"

static volatile struct pi_regs* const pir = (struct pi_regs *)0xa4600000;

#define COMMAND_SIZE 64

char *pevd_msg;
char *pevd_msg_buffer[COMMAND_SIZE+1];

watch_addr watch_addrs[MAX_WATCH];
u32 watch_index = 0; // current index in permanent watches loops at MAX_WATCH

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
    pir->read_length = len-1;
    while (dma_busy());

    // prestoreint();
    /*BYTE_T *addr = (BYTE_T*)pi_address;
    BYTE_T *src = (BYTE_T*)ram;
    for (int i = 0; i < len; i++) {
        addr[i] = src[i];
    }*/
}

void evd_reg_write(u16 reg, u32 val) {
    u32 *preg = (u32*)ED_REGS(reg);
    *preg = val;
    // pi_write(&val, ED_REGS(reg), 4);
}

u32 evd_reg_read(u16 reg) {
    u32 *preg = (u32*)ED_REGS(reg);
    u32 val = *preg;
    // pi_read(&val, ED_REGS(reg), 4);
    return val;
}

void evd_init() {
    gmemset((BYTE_T*)watch_addrs, 0x00, MAX_WATCH*sizeof(watch_addr));
    watch_index = 0;

    IO_WRITE(PI_BSD_DOM1_LAT_REG, 0x04);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, 0x0C);

    // evd_reg_write(REG_KEY, 0xAA55);
    // unlock everdrive with an inline write
    EVD_REG_WRITE(REG_KEY, 0xAA55);
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
    // pevd_msg = "USB_BUSY_READ";
    return 0;
}

BOOLEAN evd_usb_can_write() {
    u32 status = evd_reg_read(REG_USB_CFG) & (USB_STA_PWR | USB_STA_TXE);
    if (status == USB_STA_PWR) {
        return 1;
    }
    // pevd_msg = "USB_BUSY_WRITE";
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
        // pevd_msg = "BI_ERR_USB_TOUT";
        evd_reg_write(REG_USB_CFG, USB_CMD_RD_NOP);
        return BI_ERR_USB_TOUT;
    }

    return 0;
}

void evd_set_save_type(u8 type) {
    evd_reg_write(REG_RAM_CFG, type);
}

void peek(arg a, char *response, watch_type watch) {
    switch (watch) {
        case BYTE_WATCH: {
            u8 *paddr = (u8*)from_hexstr((char*)a.value, 8);
            to_hexstr(*paddr, response, 1);
            evd_usb_write(response, COMMAND_SIZE); // send back
            break; }
        case WORD_WATCH: {
            u32 *paddr = (u32*)from_hexstr((char*)a.value, 8);
            to_hexstr(*paddr, response, 4);
            evd_usb_write(response, COMMAND_SIZE); // send back
            break; }
        case HWORD_WATCH: {
            u16 *paddr = (u16*)from_hexstr((char*)a.value, 8);
            to_hexstr(*paddr, response, 2);
            evd_usb_write(response, COMMAND_SIZE); // send back
            break; }
        case FLOAT_WATCH: {
            float *paddr = (float*)from_hexstr((char*)a.value, 8);
            to_floatstr(*paddr, response, 10);
            evd_usb_write(response, COMMAND_SIZE); // send back
            break; }
        default:
            break;
    }
}

void poke(arg a, char *response, watch_type watch) {
    char *addrstr = NULL;
    char *valstr = NULL;
    split_space((char*)a.value, &addrstr, &valstr);

    if (!addrstr || !valstr) {
        response[0] = 'E';
        response[1] = 'R';
        response[2] = 'R';
        response[3] = '\0';
        evd_usb_write(response, COMMAND_SIZE); // send back
        return;
    }

    switch (watch) {
        case BYTE_WATCH: {
            // get value and address
            u8 *paddr = (u8*)from_hexstr((char*)addrstr, 8);
            u8 val = from_hexstr((char*)valstr, gstrlen((char*)valstr));
            *paddr = val;
            break; }
        case HWORD_WATCH: {
            // get value and address
            u16 *paddr = (u16*)from_hexstr((char*)addrstr, 8);
            u16 val = from_hexstr((char*)valstr, gstrlen((char*)valstr));
            *paddr = val;
            break; }
        case WORD_WATCH: {
            // get value and address
            u32 *paddr = (u32*)from_hexstr((char*)addrstr, 8);
            u32 val = from_hexstr((char*)valstr, gstrlen((char*)valstr));
            *paddr = val;
            break; }
        default:
            break;
    }

    response[0] = 'O';
    response[1] = 'K';
    response[2] = '\0';
    evd_usb_write(response, COMMAND_SIZE); // send back

}

void add_watch(arg a, char *response, watch_type watch, watch_addr *watch_addrs, u32 *watch_index) {
    watch_addr *addr = &watch_addrs[*watch_index];
    // parse input
    char *name;
    char *addrstr;
    split_space((char*)a.value, &name, &addrstr);

    if (!addrstr || !name) {
        response[0] = 'E';
        response[1] = 'R';
        response[2] = 'R';
        response[3] = '\0';
        evd_usb_write(response, COMMAND_SIZE); // send back
        return;
    }
    addr->type = watch;
    int len = gstrlen(name);
    if (len > 32) {
        len = 32;
    }
    gmemset((BYTE_T*)addr->name, '\0', 32);
    gmemcpy((BYTE_T*)name, (BYTE_T*)addr->name, len);
    addr->name[len] = '\0';
    addr->paddr = (void*)from_hexstr((char*)addrstr, 8);
    addr->enabled = TRUE;

    to_hexstr(*watch_index, response, 2);

    *watch_index += 1;

    evd_usb_write(response, COMMAND_SIZE); // send back
}

void listen(char *response, watch_addr *watch_addrs, u32 watch_index) {
    char data[COMMAND_SIZE*MAX_WATCH];
    gmemset((BYTE_T*)data, ' ', COMMAND_SIZE*MAX_WATCH);
    data[COMMAND_SIZE*MAX_WATCH-1] = '\0';

    char *pnext = data;

    // send all watches to terminal
    for (int i = 0; i < MAX_WATCH; i++) {
        if (!watch_addrs[i].enabled) {
            continue;
        }

        char *name = pnext;
        gmemcpy((BYTE_T*)watch_addrs[i].name, (BYTE_T*)name, 32);
        char *value = pnext+33;

        switch (watch_addrs[i].type) {
            case BYTE_WATCH:
                to_hexstr(*(u8*)watch_addrs[i].paddr, value, 1);
                break;
            case HWORD_WATCH:
                to_hexstr(*(u16*)watch_addrs[i].paddr, value, 2);
                break;
            case WORD_WATCH:
                to_hexstr(*(u32*)watch_addrs[i].paddr, value, 4);
                break;
            case FLOAT_WATCH:
                to_floatstr(*(float*)watch_addrs[i].paddr, value, 10);
                break;
            default:
                break;
        }
        for (int j = 0; j < COMMAND_SIZE; j++) {
            if (pnext[j] == '\0') {
                pnext[j] = ' ';
            }
        }
        pnext[COMMAND_SIZE-1] = '\n';
        pnext += COMMAND_SIZE;
    }
    data[COMMAND_SIZE*MAX_WATCH-1] = '\0';
    evd_usb_write(data, COMMAND_SIZE*MAX_WATCH); // send back
}

void evd_serial_terminal() {
    if (ed_init_done != TRUE) {
        return;
    }

    // rollover
    if (watch_index >= MAX_WATCH) {
        watch_index = 0;
    }

    char data[COMMAND_SIZE + 1];
    char response[COMMAND_SIZE + 1];
    u8 tout;

    gmemset((BYTE_T*)data, 0x00, COMMAND_SIZE+1);
    gmemset((BYTE_T*)response, 0x00, COMMAND_SIZE+1);

    if (!evd_usb_can_read()) {
        return;
    }

    tout = evd_usb_read(data, COMMAND_SIZE); // read from serial
    if (tout) {
        //pevd_msg = "BAD_READ";
        return;
    }

    // parse args
    arg a;
    if (is_arg(data, "print ")) {
        a = parse_arg(data, "print ");
        pevd_msg = (char*)pevd_msg_buffer;
        gmemcpy((BYTE_T*)a.value, (BYTE_T*)pevd_msg_buffer, COMMAND_SIZE);
        pevd_msg_buffer[COMMAND_SIZE+1] = '\0';
        response[0] = 'O';
        response[1] = 'K';
        response[2] = '\0';
        evd_usb_write(response, COMMAND_SIZE); // send back
    } else if(is_arg(data, "peekb ")) {
        // convert value to address
        a = parse_arg(data, "peekb ");
        peek(a, response, BYTE_WATCH);
    } else if(is_arg(data, "peekh ")) {
        // convert value to address
        a = parse_arg(data, "peekh ");
        peek(a, response, HWORD_WATCH);
    } else if(is_arg(data, "peekw ")) {
        // convert value to address
        a = parse_arg(data, "peekw ");
        peek(a, response, WORD_WATCH);
    } else if(is_arg(data, "peekf ")) {
        // convert value to address
        a = parse_arg(data, "peekf ");
        peek(a, response, FLOAT_WATCH);
    } else if (is_arg(data, "pokeb ")) {
        a = parse_arg(data, "pokeb ");
        poke(a, response, BYTE_WATCH);
    } else if (is_arg(data, "pokeh ")) {
        a = parse_arg(data, "pokeh ");
        poke(a, response, HWORD_WATCH);
    } else if (is_arg(data, "pokew ")) {
        a = parse_arg(data, "pokew ");
        poke(a, response, WORD_WATCH);
    } else if (is_arg(data, "watchw ")) {
        a = parse_arg(data, "watchw ");
        add_watch(a, response, WORD_WATCH, watch_addrs, &watch_index);
    } else if (is_arg(data, "watchh ")) {
        a = parse_arg(data, "watchh ");
        add_watch(a, response, HWORD_WATCH, watch_addrs, &watch_index);
    } else if (is_arg(data, "watchb ")) {
        a = parse_arg(data, "watchb ");
        add_watch(a, response, BYTE_WATCH, watch_addrs, &watch_index);
    } else if (is_arg(data, "watchf ")) {
        a = parse_arg(data, "watchf ");
        add_watch(a, response, FLOAT_WATCH, watch_addrs, &watch_index);
    } else if (is_arg(data, "listen")) {
        listen(response, watch_addrs, watch_index);
    }
}
