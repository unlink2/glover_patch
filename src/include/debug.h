#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * adaptation of https://github.com/jsdf/ed64log
 * for simple serial logging over everdrive
 * This port tries to avoid dependencies
 */

#include "typedefs.h"

#define KSEG1           0xA0000000

#define REG_BASE        0x1F800000
#define REG_FPG_CFG     0x0000
#define REG_USB_CFG     0x0004
#define REG_TIMER       0x000C
#define REG_BOOT_CFG    0x0010
#define REG_IOM_VER     0x0014
#define REG_I2C_CMD     0x0018
#define REG_I2C_DAT     0x001C

#define REG_FPG_DAT     0x0200
#define REG_USB_DAT     0x0400

#define REG_SYS_CFG     0x8000
#define REG_KEY         0x8004
#define REG_DMA_STA     0x8008
#define REG_DMA_ADDR    0x8008
#define REG_DMA_LEN     0x800C
#define REG_RTC_SET     0x8010
#define REG_RAM_CFG     0x8018
#define REG_IOM_CFG     0x801C
#define REG_SDIO        0x8020
#define REG_MCN_VER     0x8040
#define REG_SDIO_ARD    0x8200
#define REG_IOM_DAT     0x8400
#define REG_DD_TBL      0x8800

#define ED_REGS(name, reg) volatile u32* name = (u32*)(KSEG1 | REG_BASE | (reg));

void evd_init();

void evd_write_reg(u32, u32);
u16 evd_read_reg(u32);
void evd_write_msg(u32);

#endif
