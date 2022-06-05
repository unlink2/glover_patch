// Ed64 usb Code
// This module mainly contains unsafe functions
// to interface with the usb controller
use core::arch::asm;
use core::ffi::c_void;

pub enum BiError {
    BiErrUbsTout,
}

const PACKET_LEN: usize = 512;

const REG_BASE: usize = 0x1F800000;
const ED_BASE_REG: usize = 0xBF800000;
const REG_KEY: usize = 0x8004;
const REG_SYS_CFG: usize = 0x8000;
const REG_USB_CFG: usize = 0x0004;
const REG_USB_DAT: usize = 0x0400;
const REG_RAM_CFG: usize = 0x8018;

const PI_BASE_REG: usize = 0x04600000;
const PI_BSD_DOM1_LAT_REG: usize = PI_BASE_REG + 0x14;
const PI_BSD_DOM1_PWD_REG: usize = PI_BASE_REG + 0x18;
const PI_BSD_DOM1_PGS_REG: usize = PI_BASE_REG + 0x1C; /*   page size */
const PI_BSD_DOM1_RLS_REG: usize = PI_BASE_REG + 0x20;
const PI_BSD_DOM2_LAT_REG: usize = PI_BASE_REG + 0x24; /* Domain 2 latency */
const PI_BSD_DOM2_PWD_REG: usize = PI_BASE_REG + 0x28; /*   pulse width */
const PI_BSD_DOM2_PGS_REG: usize = PI_BASE_REG + 0x2C; /*   page size */
const PI_BSD_DOM2_RLS_REG: usize = PI_BASE_REG + 0x30; /*   release duration */
const PI_STATUS_REG: usize = PI_BASE_REG + 0x10;

const PI_STATUS_DMA_BUSY: usize = 1 << 0;
const PI_STATUS_IO_BUSY: usize = 1 << 1;
const PI_STATUS_ERROR: usize = 1 << 2;
const KSEG1: usize = 0xA0000000;

const SAVE_OFF: usize = 0;
const SAVE_EEP4K: usize = 1;
const SAVE_EEP16K: usize = 2;
const SAVE_SRM32K: usize = 3;
const SAVE_SRM96K: usize = 4;
const SAVE_FLASH: usize = 5;
const SAVE_SRM128K: usize = 6;
const SAVE_MPAK: usize = 8;
const SAVE_DD64: usize = 16;

const USB_LE_CFG: usize = 0x8000;
const USB_LE_CTR: usize = 0x4000;

const USB_CFG_ACT: usize = 0x0200;
const USB_CFG_RD: usize = 0x0400;
const USB_CFG_WR: usize = 0x0000;

const USB_STA_ACT: usize = 0x0200;
const USB_STA_RXF: usize = 0x0400;
const USB_STA_TXE: usize = 0x0800;
const USB_STA_PWR: usize = 0x1000;
const USB_STA_BSY: usize = 0x2000;

const USB_CMD_RD_NOP: usize = USB_LE_CFG | USB_LE_CTR | USB_CFG_RD;
const USB_CMD_RD: usize = USB_LE_CFG | USB_LE_CTR | USB_CFG_RD | USB_CFG_ACT;
const USB_CMD_WR_NOP: usize = USB_LE_CFG | USB_LE_CTR | USB_CFG_WR;
const USB_CMD_WR: usize = USB_LE_CFG | USB_LE_CTR | USB_CFG_WR | USB_CFG_ACT;

struct PiRegs {
    ram_addr: *mut c_void,
    pi_addr: *mut c_void,
    read_len: usize,
    write_len: usize,
    status: usize,
}

/// we need the registers at this specific address
const PIR: *mut PiRegs = PI_BASE_REG as *mut PiRegs;

#[derive(Copy, Clone)]
pub struct Usb {
    pub inited: bool,
}

impl Default for Usb {
    fn default() -> Self {
        Self { inited: false }
    }
}

impl Usb {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn init(&mut self) {
        init_usb_interface();
        self.inited = true;
    }

    pub fn read_usb(&self, data: &mut [u8]) -> Result<(), BiError> {
        read_usb(data)
    }

    pub fn write(data: &mut [u8]) -> Result<(), BiError> {
        write_usb(data)
    }
}

#[inline]
unsafe fn ed_regs(reg: *mut u32) -> *mut u32 {
    (KSEG1 | REG_BASE | reg as usize) as *mut u32
}

#[inline]
unsafe fn phys_to_k1(addr: *mut u32) -> *mut u32 {
    (addr as usize | KSEG1) as *mut u32
}

#[inline]
unsafe fn io_write(addr: *mut u32, data: u32) {
    *phys_to_k1(addr) = data;
}

#[inline]
unsafe fn io_read(addr: *mut u32) -> u32 {
    *phys_to_k1(addr)
}

macro_rules! cache_op {
    ($op:expr, $addr:expr, $len:expr)=>{
        let mut addr = ($addr as usize & !3) as *mut c_void;
        let len = $len;

        for _ in (0..len).rev() {
            addr = addr.offset(4);
            asm!(r#"
            .set noat
            cache {}, ({})
        "#, const $op, in(reg) addr);
        }
    }
}

#[inline]
unsafe fn data_cache_hit_writeback(addr: *mut c_void, len: usize) {
    cache_op!(0x19, addr, len);
}

#[inline]
unsafe fn data_cache_hit_writeback_invalidate(addr: *mut c_void, len: usize) {
    cache_op!(0x15, addr, len);
}

#[inline]
unsafe fn dma_busy() -> bool {
    (*PIR).status & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY) != 0
}

unsafe fn pi_read(ram: *mut c_void, mut pi_addr: *mut c_void, len: usize) {
    pi_addr = (pi_addr as usize & 0x1FFFFFFF) as *mut c_void;
    data_cache_hit_writeback_invalidate(ram, len);

    while dma_busy() {}
    io_write(PI_STATUS_REG as *mut u32, 3);
    (*PIR).ram_addr = ram;
    (*PIR).pi_addr = pi_addr;
    (*PIR).write_len = len - 1;
    while dma_busy() {}
}

unsafe fn pi_write(ram: *mut c_void, mut pi_addr: *mut c_void, len: usize) {
    pi_addr = (pi_addr as usize & 0x1FFFFFFF) as *mut c_void;
    data_cache_hit_writeback(ram, len);

    while dma_busy() {}
    io_write(PI_STATUS_REG as *mut u32, 3);
    (*PIR).ram_addr = ram;
    (*PIR).pi_addr = pi_addr;
    (*PIR).read_len = len - 1;
    while dma_busy() {}
}

unsafe fn evd_reg_write(reg: *mut u32, val: u32) {
    let preg = ed_regs(reg);
    *preg = val;
}

unsafe fn evd_reg_read(reg: *mut u32) -> u32 {
    let preg = ed_regs(reg);
    *preg
}

unsafe fn evd_init() {
    io_write(PI_BSD_DOM1_LAT_REG as *mut u32, 0x04);
    io_write(PI_BSD_DOM1_PWD_REG as *mut u32, 0x0C);

    // unlock everdrive
    evd_reg_write(REG_KEY as *mut u32, 0xAA55);
    evd_reg_write(REG_SYS_CFG as *mut u32, 0);

    // TODO
    evd_usb_init();
    evd_set_save_type(SAVE_OFF);
}

unsafe fn evd_usb_init() {
    let mut buffer: [u8; PACKET_LEN] = [0; PACKET_LEN];

    evd_reg_write(REG_USB_CFG as *mut u32, USB_CMD_RD_NOP as u32);

    // flush fifo buffer
    while evd_usb_can_read() {
        let response = evd_usb_read(&mut buffer);
        if response.is_err() {
            break;
        }
    }
}

unsafe fn evd_usb_can_read() -> bool {
    let status = evd_reg_read(REG_USB_CFG as *mut u32) & (USB_STA_PWR as u32 | USB_STA_RXF as u32);

    status == USB_STA_PWR as u32
}

unsafe fn evd_usb_can_write() -> bool {
    let status = evd_reg_read(REG_USB_CFG as *mut u32) & (USB_STA_PWR as u32 | USB_STA_RXF as u32);

    status == USB_STA_PWR as u32
}

unsafe fn evd_usb_read(buffer: &mut [u8]) -> Result<(), BiError> {
    let mut response = Ok(());
    let mut len = buffer.len();
    let mut buffer = buffer.as_mut_ptr() as *mut u8;

    while len > 0 {
        let mut blen = PACKET_LEN;
        if blen > len {
            blen = len;
        }
        let baddr = PACKET_LEN - blen;

        evd_reg_write(REG_USB_CFG as *mut u32, USB_CMD_RD as u32 | baddr as u32);

        response = evd_usb_busy();

        if response.is_err() {
            break;
        }
        pi_read(
            buffer as *mut c_void,
            ed_regs((REG_USB_DAT + baddr) as *mut u32) as *mut c_void,
            blen,
        );

        buffer = buffer.add(blen);
        len -= blen;
    }

    response
}

unsafe fn evd_usb_write(buffer: &mut [u8]) -> Result<(), BiError> {
    let mut response = Ok(());
    let mut len = buffer.len();
    let mut buffer = buffer.as_mut_ptr() as *mut u8;

    evd_reg_write(REG_USB_CFG as *mut u32, USB_CMD_WR_NOP as u32);
    while len > 0 {
        let mut blen = PACKET_LEN;
        if blen > len {
            blen = len;
        }
        let baddr = PACKET_LEN - blen;
        pi_write(
            buffer as *mut c_void,
            ed_regs((REG_USB_DAT + baddr) as *mut u32) as *mut c_void,
            blen,
        );
        buffer = buffer.add(PACKET_LEN);

        evd_reg_write(REG_USB_CFG as *mut u32, (USB_CMD_WR | baddr) as u32);

        response = evd_usb_busy();
        if response.is_err() {
            break;
        }

        len -= blen;
    }

    response
}

unsafe fn evd_usb_busy() -> Result<(), BiError> {
    let mut tout = 0;
    while evd_reg_read(REG_USB_CFG as *mut u32) & USB_STA_ACT as u32 != 0 {
        // delay
        if tout != 8192 {
            tout += 1;
            continue;
        }
        evd_reg_write(REG_USB_CFG as *mut u32, USB_CMD_RD_NOP as u32);

        return Err(BiError::BiErrUbsTout);
    }

    Ok(())
}

unsafe fn evd_set_save_type(stype: usize) {
    evd_reg_write(REG_RAM_CFG as *mut u32, stype as u32);
}

pub fn init_usb_interface() {
    unsafe {
        evd_init();
    }
}

pub fn read_usb(data: &mut [u8]) -> Result<(), BiError> {
    unsafe {
        if let Err(e) = evd_usb_busy() {
            Err(e)
        } else {
            evd_usb_read(data)
        }
    }
}

pub fn write_usb(data: &mut [u8]) -> Result<(), BiError> {
    unsafe {
        if let Err(e) = evd_usb_busy() {
            Err(e)
        } else {
            evd_usb_write(data)
        }
    }
}
