use super::color::Color;
use super::menu::*;
use super::render::{to_decimal, Drawable, RenderContext, Widget};
use core::ffi::c_void;
use core::marker::PhantomData;

/**
 * A tmter that counts time based on framerate
 */
pub struct Timer<T>
where
    T: Copy + Clone,
{
    x: isize,
    y: isize,
    pub frames: u32,
    pub active: bool,
    pub framerate: u32,
    phantom: PhantomData<T>,
    buffer: [u8; 32],
}

impl<T> Timer<T>
where
    T: Copy + Clone,
{
    pub fn new(x: isize, y: isize) -> Self {
        Self {
            x,
            y,
            frames: 0,
            active: false,
            framerate: 20,
            buffer: [0; 32],
            phantom: PhantomData,
        }
    }

    pub fn inc(&mut self) {
        self.frames += 1;
    }

    pub fn set(&mut self, frames: u32) {
        self.frames = frames;
    }

    fn puts_time(time: u32, buffer: &mut [u8]) {
        let mut start = 0;

        if time < 10 {
            buffer[start] = b'0';
            start = 1;
        }

        // seconds
        to_decimal(time as isize, &mut buffer[start..]);
    }
}

impl<T> Drawable<T> for Timer<T>
where
    T: Copy + Clone,
{
    fn update(&mut self, _data: T) {}

    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        if self.active {
            // minutes
            Self::puts_time(self.frames / self.framerate / 60, &mut self.buffer[..]);
            self.buffer[2] = b':';

            // seconds
            Self::puts_time(self.frames / self.framerate % 60, &mut self.buffer[3..]);
            self.buffer[5] = b':';

            // frames
            Self::puts_time(self.frames % 20, &mut self.buffer[6..]);

            ctxt.putsu8(&self.buffer, self.x, self.y);
        }
    }
}

impl<T> Widget<T> for Timer<T>
where
    T: Copy + Clone,
{
    fn toggle(&mut self, _data: T) {
        self.active = !self.active
    }

    fn active(&self) -> bool {
        self.active
    }
}
