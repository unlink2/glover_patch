#include "include/debug.h"
#include "include/render.h"
#include "include/utility.h"



void evd_init() {
    // volatile u8 val;
    ED_REGS(regs_ptr);
    regs_ptr[0];
    regs_ptr[REG_KEY] = 0x1234;  // unlock everdrive
}

void evd_write_reg(u8 reg, u16 val) {
    ED_REGS(regs_ptr);
    // volatile u8 tmp;
    regs_ptr[0];
    regs_ptr[reg] = val;
}

u16 evd_read_reg(u8 reg) {
    ED_REGS(regs_ptr);
    // volatile u32 tmp;
    regs_ptr[0];
    return regs_ptr[reg];
}


void evd_write_msg(u8 dat) {
    evd_write_reg(REG_MSG, dat);
}

void debug_print(u32 value) {
    get_ptr(char, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    to_hexstr(value, string_buffer, 4);
    puts_xy(string_buffer, 5, 5);
}
