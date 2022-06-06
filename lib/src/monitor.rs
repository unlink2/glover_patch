use crate::input::InputHandler;
use crate::keyboard::{self, Keyboard, HEX};
use embedgdb::Stream;

use super::color::Color;
use super::menu::*;
use super::render::{Drawable, RenderContext, Widget};
use core::ffi::c_void;
use embedgdb::{BufferedStream, Parser};

/**
 * This is a memory monitor
 * that can browse wram
 */
pub struct Monitor<T>
where
    T: Copy + Clone,
{
    x: isize,
    y: isize,
    pub active: bool,

    addr: *mut c_void,
    rows: usize,
    bytes_per_row: usize,
    toggle_timer_max: u16,
    toggle_timer: u16,
    close_action: EntryTypes<T>,
    back_action: EntryTypes<T>,
    open_action: EntryTypes<T>,
    cursor_x: usize,
    cursor_y: usize,
    addr_buffer: [u8; 8],
    keyboard: Keyboard<'static, T>,
    ascii_mode: bool,
}

impl<T> Monitor<T>
where
    T: Copy + Clone,
{
    pub fn new(
        x: isize,
        y: isize,
        open_action: EntryTypes<T>,
        close_action: EntryTypes<T>,
        back_action: EntryTypes<T>,
    ) -> Self {
        Self {
            x,
            y,
            active: true,
            addr: 0x80000000 as *mut c_void,
            bytes_per_row: 8,
            rows: 8,
            toggle_timer_max: 10,
            toggle_timer: 0,
            open_action,
            close_action,
            back_action,
            cursor_x: 0,
            cursor_y: 0,
            addr_buffer: [0; 8],
            keyboard: Keyboard::new(x, y, &HEX),
            ascii_mode: false,
        }
    }

    pub fn left(&mut self) {
        if !self.keyboard.active() {
            if self.cursor_x == 0 {
                self.cursor_x = self.bytes_per_row - 1;
            } else {
                self.cursor_x -= 1;
            }
        } else {
            self.keyboard.left();
        }
    }

    pub fn right(&mut self) {
        if !self.keyboard.active() {
            self.cursor_x += 1;
            if self.cursor_x >= self.bytes_per_row {
                self.cursor_x = 0;
            }
        } else {
            self.keyboard.right();
        }
    }

    pub fn up(&mut self) {
        if !self.keyboard.active() {
            if self.cursor_y == 0 {
                self.cursor_y = self.rows - 1;
            } else {
                self.cursor_y -= 1;
            }
        } else {
            self.keyboard.up();
        }
    }

    pub fn down(&mut self) {
        if !self.keyboard.active() {
            self.cursor_y += 1;
            if self.cursor_y >= self.rows {
                self.cursor_y = 0;
            }
        } else {
            self.keyboard.down();
        }
    }

    pub fn inc_addr(&mut self) {
        if !self.keyboard.active() {
            self.addr = unsafe { self.addr.add(self.bytes_per_row * self.rows) };
        }
    }

    pub fn dec_addr(&mut self) {
        if !self.keyboard.active() {
            self.addr = unsafe { self.addr.sub(self.bytes_per_row * self.rows) };
        }
    }

    pub fn inc_value(&mut self) {
        if !self.keyboard.active() {
            let addr = unsafe {
                self.addr
                    .add(self.calc_offset(self.cursor_x, self.cursor_y))
            };

            unsafe {
                *(addr as *mut u8) += 1;
            }
        }
    }

    pub fn dec_value(&mut self) {
        if !self.keyboard.active() {
            let addr = unsafe {
                self.addr
                    .add(self.calc_offset(self.cursor_x, self.cursor_y))
            };

            unsafe {
                *(addr as *mut u8) -= 1;
            }
        }
    }

    pub fn select(&mut self) {
        if self.keyboard.active() {
            self.keyboard.select(&mut self.addr_buffer);
        }
    }

    pub fn enter(&mut self) {
        if self.keyboard.active() {
            self.keyboard.enter();
        } else {
            self.value_input();
        }
    }

    pub fn toggle_ascii(&mut self) {
        self.ascii_mode = !self.ascii_mode;
    }

    pub fn addr_input(&mut self) {
        self.keyboard.reset(&mut self.addr_buffer, 0);
        self.keyboard.active = true;
    }

    pub fn value_input(&mut self) {
        self.keyboard.reset(&mut self.addr_buffer, 1);
        self.keyboard.active = true;
    }

    pub fn open(&mut self, data: T) {
        self.active = true;
        self.open_action.activate(data);
    }

    pub fn close(&mut self, data: T) {
        self.active = false;
        self.close_action.activate(data);
    }

    pub fn back(&mut self, data: T) {
        if self.keyboard.active() {
            self.keyboard.back(&mut self.addr_buffer);
        } else {
            self.toggle_timer = 0;
            self.back_action.activate(data);
        }
    }

    fn calc_offset(&self, x: usize, y: usize) -> usize {
        y * self.bytes_per_row + x
    }
}

impl<T> Drawable<T> for Monitor<T>
where
    T: Copy + Clone,
{
    fn update(&mut self, data: T, input: &InputHandler) {
        if self.toggle_timer > 0 {
            self.toggle_timer -= 1;
        }

        if !self.active {
            return;
        }

        if self.keyboard.active() {
            self.keyboard.update(data, input);
        } else if self.keyboard.enter {
            self.keyboard.enter = false;
            match self.keyboard.tag {
                0 => {
                    // this input cannot fail because of the restricted keyboard input
                    let addr = Parser::from_hexu(&self.addr_buffer).unwrap_or(0);
                    self.addr = addr as *mut c_void;
                }
                _ => {
                    let value = Parser::from_hexu(&self.addr_buffer).unwrap_or(0) as u8;
                    let addr = unsafe {
                        self.addr
                            .add(self.calc_offset(self.cursor_x, self.cursor_y))
                    };
                    unsafe { *(addr as *mut u8) = value };
                }
            }
        }
    }

    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        if self.keyboard.active() && self.active {
            self.keyboard.draw_buffer(ctxt, &self.addr_buffer);
            self.keyboard.draw(ctxt);
        } else {
            let mut stream = BufferedStream::new();
            let _ = Parser::to_hexu(&(self.addr as usize).to_be_bytes(), &mut stream); // this should not fail!
            let _ = stream.write(0); // make sure to terminate the string
            ctxt.putsu8(&stream.buffer, self.x, self.y);

            for r in 0..self.rows {
                let offset_hex =
                    Parser::to_hex_tuple(unsafe { self.addr.add(self.calc_offset(0, r)) as u8 });

                let addr_offset = [offset_hex.0 as char, offset_hex.1 as char];
                let y = self.y as isize + ctxt.char_height() * (r as isize + 1);

                ctxt.set_color(Color::new(0xFF, 0xFF, 0x00, 0xFF));
                ctxt.cputs(&addr_offset, self.x as isize, y);

                for c in 0..self.bytes_per_row {
                    let address = unsafe { self.addr.add(self.calc_offset(c, r)) };
                    let value = if address as usize >= 0x80000000 && (address as usize) < 0x807FFFFF
                    {
                        unsafe { *(address as *const u8) }
                    } else {
                        0
                    };

                    let x = self.x as isize + (2 * ctxt.char_width() + 4) * (c as isize + 1);

                    if self.cursor_x == c && self.cursor_y == r {
                        ctxt.set_color(Color::new(0xFF, 0x00, 0x00, 0xFF));
                    }

                    let value_str = if self.ascii_mode {
                        [ctxt.convert(value) as char, 0 as char]
                    } else {
                        let value_hex = Parser::to_hex_tuple(value);
                        [value_hex.0 as char, value_hex.1 as char]
                    };
                    ctxt.cputs(&value_str, x, y);
                }
            }
        }
    }
}

impl<T> Widget<T> for Monitor<T>
where
    T: Copy + Clone,
{
    fn toggle(&mut self, data: T) {
        if self.toggle_timer > 0 {
            return;
        }

        self.toggle_timer = self.toggle_timer_max;
        self.keyboard.active = false;
        if self.active {
            self.close(data);
        } else {
            self.open(data);
        }
    }

    fn active(&self) -> bool {
        self.active
    }
}
