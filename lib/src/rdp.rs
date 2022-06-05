use super::font::*;
use super::interrupt::{DisableIntFn, EnableIntFn};
use super::memory::umemset;
use super::render::RenderContext;
use core::ffi::c_void;

/**
 * This is a game-indipendand
 * font renderer
 * it works by writing rdp commands into a buffer and sending them
 *  TODO WIP not functions. This has a lower prioerity just implement game-specific rendering for
 *  now
 */

/// Dp Command registers
/// as a struct for easier access
#[repr(C)]
pub struct DpCmdRegisters {
    start: *const c_void,
    end: *const c_void,
    current: *const c_void,
    status: u32,
    clock: u32,
    buf_busy: u32,
    pipe_busy: u32,
    tmem: u32,
}

impl DpCmdRegisters {
    /// converts the actual dp register location into a pointer
    /// of type DpCmdRegisters and returns it
    pub fn new() -> *mut Self {
        unsafe {
            core::mem::transmute::<*const c_void, *mut DpCmdRegisters>(0xA4100000 as *mut c_void)
        }
    }
}

// TODO remove hard coded ptrs
pub struct RdpFontRendererContext<'a> {
    buffer: *mut u32,
    pub offset: usize,
    start: usize,
    size: usize,
    registers: *mut DpCmdRegisters,
    on_disable: DisableIntFn,
    on_enable: EnableIntFn,
    font: &'a dyn GenericFont<'a>,
}

impl<'a> RdpFontRendererContext<'a> {
    pub fn new(
        buffer: *mut u32,
        size: usize,
        on_enable: EnableIntFn,
        on_disable: DisableIntFn,
        font: &'a dyn GenericFont<'a>,
    ) -> Self {
        let mut rdp = Self {
            buffer,
            size,
            offset: 0,
            start: 0,
            registers: DpCmdRegisters::new(),
            on_disable,
            on_enable,
            font,
        };
        unsafe {
            rdp.clear_buffer(rdp.size);
        }
        rdp
    }

    // the values are registrs and do change in the loop
    #[allow(clippy::while_immutable_condition)]
    #[inline(always)]
    unsafe fn wait_pipe(&mut self, other: u32) {
        while ((*self.registers).status as u32 & (0x600 | other)) > 0 {}
    }

    /// clears the buffer starting at offset
    #[inline(always)]
    unsafe fn clear_buffer(&mut self, size: usize) {
        umemset(
            self.buffer.add(self.offset) as *mut u8,
            0,
            size * core::mem::size_of::<u32>(),
        );
    }

    unsafe fn send_cmds(&mut self) {
        // self.align();

        let start = self.buffer.add(self.start);
        let end = self.buffer.add(self.offset);

        if start == end {
            return;
        }

        // wait for rdp
        self.wait_pipe(0);

        // no interrupts to prevent conflicts
        // (self.on_disable)();
        let previ = (self.on_disable)();

        // set flush flag
        (*self.registers).status = 0x15;
        // wait again to flush
        self.wait_pipe(0);

        // start dma
        // & 0x00FFFFFF -> converts to physical address for dma
        (*self.registers).start = (start as u32) as *const c_void;
        (*self.registers).end = (end as u32) as *const c_void;
        // dma busy; pipe busy
        self.wait_pipe(0b101000000);

        (self.on_enable)(previ);
    }

    /// # Safety
    #[inline(always)]
    unsafe fn write(&mut self, value: u32) {
        *self.buffer.add(self.offset) = value;
        self.offset += 1;
    }

    /// # Safety
    pub unsafe fn sync_full(&mut self) {
        self.write(0x29000000);
        self.write(0x00000000);
    }

    /// # Safety
    pub unsafe fn sync_pipe(&mut self) {
        self.write(0x27000000);
        self.write(0x00000000);
    }

    /// # Safety
    pub unsafe fn sync_load(&mut self) {
        self.write(0x31000000);
        self.write(0x00000000);
    }

    /// # Safety
    pub unsafe fn sync_tile(&mut self) {
        self.write(0x28000000);
        self.write(0x00000000);
    }

    /// # Safety
    pub unsafe fn draw_primitives(&mut self) {
        self.write(0xEFB000FF);
        self.write(0x00004004);
    }

    /// # Safety
    /// draw a rectangle
    pub unsafe fn draw_rect(&mut self, color: u32, tx: i32, ty: i32, bx: i32, by: i32) {
        // set color
        self.write(0xF7000000);
        self.write(color);

        // set location
        self.write(0xF6000000 | ((bx as u32) << 14) | ((by as u32) << 2));
        self.write(((tx as u32) << 14) | ((ty as u32) << 2));
    }

    /// # Safety
    /// set rdp to texture mode
    pub unsafe fn texture_mode(&mut self) {
        self.write(0x2F002830);
        self.write(0x00404040);
        self.write(0x3C0000C1);
        self.write(0x0F2001FF);
    }

    /// # Safety
    /// Load a tile
    pub unsafe fn load_tile(&mut self, font: &dyn GenericFont, offset: usize) {
        // sync
        self.write(0x27000000);
        self.write(0x00000000);

        // set texture image
        self.write(0x3D100007);
        self.write(font.data().as_ptr().add(offset) as u32);

        self.write(0x31000000);
        self.write(0x00000000);
    }

    /// # Safety
    /// Draws a tile directly to the rdp
    #[allow(clippy::identity_op)]
    pub unsafe fn draw_tile(&mut self, xh: i32, yh: i32, w: i32, h: i32) {
        // set tile
        self.write(0x35100400); //  0<<2,0<<2, 0, 7<<2,7<<2, sl 0.0 tl 0.0, sh 7.0 th 7.0
        self.write(0x00000000);

        // set texture rect
        self.write(0x34000000);
        self.write(0x0001C01C);

        // draw rect
        self.write(0x24000000 | (xh as u32 + w as u32) << 14 | (yh as u32 + h as u32) << 2); // xh+w == xl; yh+h == yl
        self.write(0x00000000 | (xh as u32) << 14 | (yh as u32) << 2);
        self.write(0x00000000); // S 0.0 T 0.0
        self.write(0x04000400); // scale factor 1:1
    }

    /// # Safety
    /// aligsn write to size of 8
    pub unsafe fn align(&mut self) {
        while self.offset % 8 != 0 {
            self.write(0);
        }
    }

    fn draw_char(&mut self, c: char, x: isize, y: isize) {
        if c == '\0' {
            return;
        }

        let offset = c as usize * CHAR_W * CHAR_H;
        unsafe {
            self.load_tile(self.font, offset);
            self.draw_tile(x as i32, y as i32, CHAR_W as i32, CHAR_H as i32);
        }
    }
}

impl RenderContext for RdpFontRendererContext<'_> {
    // TODO the current way of sending the buffer
    // will allow it to overflow happily
    // and there is now way to reset it half-way through a write
    fn draw(&mut self) {
        // send previous dl
        unsafe {
            self.send_cmds();
        }
        // check bounds of dl
        if self.start > self.size {
            self.offset = 0;
            self.start = 0;
        }

        self.start = self.offset;
    }

    fn puts(&mut self, s: &str, x: isize, y: isize) {
        let mut current_x = x;
        unsafe {
            self.texture_mode();
        }
        for c in s.chars() {
            self.draw_char(c, current_x, y);
            current_x += CHAR_W as isize;
        }
    }

    fn cputs(&mut self, s: &[char], x: isize, y: isize) {
        let mut current_x = x;
        unsafe {
            self.texture_mode();
        }
        for c in s {
            if *c == '\0' {
                break;
            }
            self.draw_char(*c, current_x, y);
            current_x += CHAR_W as isize;
        }
    }

    fn putsu8(&mut self, s: &[u8], x: isize, y: isize) {
        let mut current_x = x;
        unsafe {
            self.texture_mode();
        }
        for c in s {
            if *c == b'\0' {
                break;
            }
            self.draw_char(*c as char, current_x, y);
            current_x += CHAR_W as isize;
        }
    }
}
