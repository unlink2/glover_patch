#include "include/debug.h"
#include "include/render.h"
#include "include/utility.h"



void evd_init() {
    // volatile u8 val;
    ED_REGS(regs_ptr, REG_KEY);
    *regs_ptr = 0x1234;  // unlock everdrive
}

void evd_write_reg(u32 reg, u32 val) {
    /* ED_REGS(regs_ptr, reg);
    // volatile u8 tmp;
    regs_ptr[0];
    regs_ptr[reg] = val;*/
}

u16 evd_read_reg(u32 reg) {
    /* ED_REGS(regs_ptr, reg);
    // volatile u32 tmp;
    regs_ptr[0];
    return regs_ptr[reg];*/
    return 0;
}


void evd_write_msg(u32 dat) {
    // evd_write_reg(REG_MSG, dat);
}

