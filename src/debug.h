#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * adaptation of https://github.com/jsdf/ed64log and the Everdrive usb sample
 */

#include "typedefs.h"
#include "utility.h"
#include "memwatch.h"

#define KSEG1 0xA0000000

#define BI_ERR_I2C_CMD     0xB0
#define BI_ERR_I2C_TOUT    0xB1
#define BI_ERR_USB_TOUT    0xB2
#define BI_ERR_MCN_CFG     0xB3
#define BI_ERR_IOM_CFG     0xB4

#define BI_ROM_ADDR    (KSEG1 | 0x10000000)
#define BI_RAM_ADDR    (KSEG1 | 0x08000000)

#define BI_ROM_SLEN     0x20000
#define BI_RAM_SLEN     0x100

#define BI_DISK_SPD_LO  0
#define BI_DISK_SPD_HI  1

#define BI_BCFG_BOOTMOD 1
#define BI_BCFG_SD_INIT 2
#define BI_BCFG_SD_TYPE 4
#define BI_BCFG_GAMEMOD 8
#define BI_BCFG_CICLOCK 0x8000

#define BI_DD_TBL_SIZE  2048
#define BI_DD_PGE_SIZE  0x8000


#define BI_MIN_USB_SIZE 16

#define SAVE_OFF        0
#define SAVE_EEP4K      1
#define SAVE_EEP16K     2
#define SAVE_SRM32K     3
#define SAVE_SRM96K     4
#define SAVE_FLASH      5
#define SAVE_SRM128K    6
#define SAVE_MPAK       8
#define SAVE_DD64       16

#define REG_BASE        0x1F800000
#define ED_BASE_REG     0xBF800000
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

#define DMA_STA_BUSY    0x0001
#define DMA_STA_ERROR   0x0002
#define DMA_STA_LOCK    0x0080

#define SD_CFG_BITLEN   0x000F
#define SD_CFG_SPD      0x0010
#define SD_STA_BUSY     0x0080

#define CFG_BROM_ON     0x0001
#define CFG_REGS_OFF    0x0002
#define CFG_SWAP_ON     0x0004

#define FPG_CFG_NCFG    0x0001
#define FPG_STA_CDON    0x0001
#define FPG_STA_NSTAT   0x0002

#define I2C_CMD_DAT     0x10
#define I2C_CMD_STA     0x20
#define I2C_CMD_END     0x30

#define IOM_CFG_SS      0x0001
#define IOM_CFG_RST     0x0002
#define IOM_CFG_ACT     0x0080
#define IOM_STA_CDN     0x0001

#define USB_LE_CFG      0x8000
#define USB_LE_CTR      0x4000

#define USB_CFG_ACT     0x0200
#define USB_CFG_RD      0x0400
#define USB_CFG_WR      0x0000

#define USB_STA_ACT     0x0200
#define USB_STA_RXF     0x0400
#define USB_STA_TXE     0x0800
#define USB_STA_PWR     0x1000
#define USB_STA_BSY     0x2000

#define USB_CMD_RD_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD)
#define USB_CMD_RD      (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD | USB_CFG_ACT)
#define USB_CMD_WR_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR)
#define USB_CMD_WR      (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR | USB_CFG_ACT)

#define REG_LAT 0x04
#define REG_PWD 0x04

#define ROM_LAT 0x40
#define ROM_PWD 0x12

// ifdef to stop warning in unit tests
// unit tests cannot call ED functions anyway
#ifdef __LP64__
#define ED_REGS(reg) 0
#else
#define ED_REGS(reg) (KSEG1 | REG_BASE | (reg))
#endif

#define EVD_REG_WRITE(reg, val) {u32 *preg = (u32*)(ED_BASE_REG | (reg));*preg = val;}
// TODO this read does not always work
#define EVD_REG_READ(reg, val) {u32 *preg = (u32*)(ED_BASE_REG | (reg)); val = *preg;}

#define PI_BASE_REG 0x04600000
#define PI_BSD_DOM1_LAT_REG (PI_BASE_REG+0x14)
#define PI_BSD_DOM1_PWD_REG (PI_BASE_REG+0x18)
#define PI_BSD_DOM1_PGS_REG (PI_BASE_REG+0x1C)    /*   page size */
#define PI_BSD_DOM1_RLS_REG (PI_BASE_REG+0x20)
#define PI_BSD_DOM2_LAT_REG (PI_BASE_REG+0x24)    /* Domain 2 latency */
#define PI_BSD_DOM2_PWD_REG (PI_BASE_REG+0x28)    /*   pulse width */
#define PI_BSD_DOM2_PGS_REG (PI_BASE_REG+0x2C)    /*   page size */
#define PI_BSD_DOM2_RLS_REG (PI_BASE_REG+0x30)    /*   release duration */
#define PI_STATUS_REG (PI_BASE_REG+0x10)

#define PI_STATUS_DMA_BUSY ( 1 << 0 )
#define PI_STATUS_IO_BUSY  ( 1 << 1 )
#define PI_STATUS_ERROR    ( 1 << 2 )

#define PHYS_TO_K1(x) ((u32)(x)|KSEG1)        /* physical to kseg1 */
#define IO_WRITE(addr, data) (*(volatile u32 *)PHYS_TO_K1(addr) = (u32)(data))
#define IO_READ(addr) (*(volatile u32 *)PHYS_TO_K1(addr))

extern char *pevd_msg;

typedef struct pi_regs {
    void *ram_address;
    unsigned long pi_address;
    unsigned long read_length;
    unsigned long write_length;
    unsigned long status;
} pi_regs;

BOOLEAN dma_busy();
void pi_write(void *, unsigned long, unsigned long);
void pi_write(void *, unsigned long, unsigned long);

void evd_reg_write(u16, u32);
u32 evd_reg_read(u16);
void evd_init();

void evd_usb_init();

BOOLEAN evd_usb_can_read();
BOOLEAN evd_usb_can_write();

BOOLEAN evd_usb_write(void *, u32);
BOOLEAN evd_usb_read(void *, u32);
BOOLEAN evd_usb_busy();

void evd_set_save_type(u8);

void evd_serial_terminal(memwatch *);

#endif
