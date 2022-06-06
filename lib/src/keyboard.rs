use core::marker::PhantomData;

use crate::{color::Color, input::InputHandler};

use super::render::{Drawable, RenderContext, Widget};

pub static HEX_ROW1: [u8; 5] = [b'0', b'1', b'2', b'3', b'4'];
pub static HEX_ROW2: [u8; 5] = [b'5', b'6', b'7', b'8', b'9'];
pub static HEX_ROW3: [u8; 5] = [b'A', b'C', b'D', b'E', b'F'];
pub static HEX: [&[u8]; 3] = [&HEX_ROW1, &HEX_ROW2, &HEX_ROW3];

/**
 * Represents a simple keyboard
 * that can be rendered to any render context
 */
pub struct Keyboard<'a, T>
where
    T: Copy + Clone,
{
    pos_x: isize,
    pos_y: isize,
    cursor_x: usize,
    cursor_y: usize,
    position: usize,
    pub active: bool,
    pub enter: bool,
    pub tag: u8, // 8 bit tag to identify the input type
    grid: &'a [&'a [u8]],
    phantom: PhantomData<T>,
}

impl<'a, T> Keyboard<'a, T>
where
    T: Copy + Clone,
{
    pub fn new(pos_x: isize, pos_y: isize, grid: &'a [&'a [u8]]) -> Self {
        Self {
            pos_x,
            pos_y,
            cursor_x: 0,
            cursor_y: 0,
            active: false,
            enter: false,
            grid,
            tag: 0,
            position: 0,
            phantom: PhantomData,
        }
    }

    pub fn down(&mut self) {
        if self.cursor_y < self.grid.len() {
            self.cursor_y += 1;
        } else {
            self.cursor_y = 0;
        }

        self.cursor_x = usize::min(self.cursor_x, self.grid[self.cursor_y].len());
    }

    pub fn up(&mut self) {
        if self.cursor_y > 0 {
            self.cursor_y -= 1;
        } else {
            self.cursor_y = self.grid.len() - 1;
        }
        self.cursor_x = usize::min(self.cursor_x, self.grid[self.cursor_y].len());
    }

    pub fn left(&mut self) {
        if self.cursor_x > 0 {
            self.cursor_x -= 1;
        } else {
            self.cursor_x = self.grid[self.cursor_y].len() - 1;
        }
    }

    pub fn right(&mut self) {
        if self.cursor_x < self.grid[self.cursor_y].len() {
            self.cursor_x += 1;
        } else {
            self.cursor_x = 0;
        }
    }

    pub fn select(&mut self, buffer: &mut [u8]) {
        if self.position < buffer.len() {
            buffer[self.position] = self.grid[self.cursor_y][self.cursor_x];
            self.position += 1;
        }
    }

    pub fn enter(&mut self) {
        self.active = false;
        self.enter = true;
    }

    pub fn back(&mut self, buffer: &mut [u8]) {
        if self.position > 0 {
            self.position -= 1;
            buffer[self.position] = b'\0';
        } else {
            self.active = false;
            self.enter = false;
        }
    }

    pub fn reset(&mut self, buffer: &mut [u8], tag: u8) {
        self.position = 0;
        self.tag = tag;
        buffer.fill(0);
    }

    pub fn draw_buffer(&mut self, ctxt: &mut dyn RenderContext, buffer: &[u8]) {
        ctxt.putsu8(buffer, self.pos_x, self.pos_y);
    }
}

impl<T> Drawable<T> for Keyboard<'_, T>
where
    T: Copy + Clone,
{
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        for (y, row) in self.grid.iter().enumerate() {
            for (x, col) in row.iter().enumerate() {
                if y == self.cursor_y && x == self.cursor_x {
                    ctxt.set_color(Color::new(0xFF, 0x00, 0x00, 0xFF));
                }
                let cstr = [*col, b'\0'];
                ctxt.putsu8(
                    &cstr,
                    self.pos_x + x as isize * ctxt.char_width(),
                    self.pos_y + y as isize * ctxt.char_height() + 2 * ctxt.char_height(),
                );
            }
        }
    }

    fn update(&mut self, _data: T, input: &InputHandler) {}
}

impl<T> Widget<T> for Keyboard<'_, T>
where
    T: Copy + Clone,
{
    fn active(&self) -> bool {
        self.active
    }

    fn toggle(&mut self, _data: T) {
        self.active = !self.active
    }
}
