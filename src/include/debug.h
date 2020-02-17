#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * adaptation of https://github.com/jsdf/ed64log
 * for simple serial logging over everdrive
 * This port tries to avoid dependencies
 */

#include "typedefs.h"

#define REG_CFG 0
#define REG_STATUS 1
#define REG_DMA_LEN 2
#define REG_DMA_RAM_ADDR 3
#define REG_MSG 4
#define REG_DMA_CFG 5
#define REG_SPI 6
#define REG_SPI_CFG 7
#define REG_KEY 8
#define REG_SAV_CFG 9
#define REG_SEC 10
#define REG_VER 11

#define REG_CFG_CNT 16
#define REG_CFG_DAT 17
#define REG_MAX_MSG 18
#define REG_CRC 19

#define ED_CFG_SDRAM_ON 0
#define ED_CFG_SWAP 1
#define ED_CFG_WR_MOD 2
#define ED_CFG_WR_ADDR_MASK 3

#define ED_REGS(name) volatile u32* name = (u32*)0xA8040000;

void evd_init();

void evd_write_reg(u8, u16);
u16 evd_read_reg(u8);
void evd_write_msg(u8 dat);

void debug_print(u32 value);

#endif 
