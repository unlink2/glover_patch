use crate::color::Color;

/// The following usage only applies when writing a custom rendering pipeline
/// if the game's font renderer is used you should be able to ignore the update function completely
/// for a sample see the WIP rdp font rendering context
pub trait RenderContext {
    /// this function should flush the
    /// last draw operations to the screen and
    /// clear the buffer
    fn draw(&mut self) {}

    /// the put and draw functions should write to
    /// an internal buffer and that gets drawn to
    /// the screen during update
    /// puts a rust string slice to the screen
    fn puts(&mut self, s: &str, x: isize, y: isize);

    /// puts an array of chars
    fn cputs(&mut self, s: &[char], x: isize, y: isize);

    /// puts an array of raw bytes (c-like, null terminated )
    fn putsu8(&mut self, s: &[u8], x: isize, y: isize);

    /// if the renderer does not support all ascii
    /// characters override this convert method to allow dynamic
    /// conversion of values to printable characters
    fn convert(&self, c: u8) -> u8 {
        c
    }

    /// optional method, can be ignored
    /// if not supported
    /// return false whenever color is not supported
    fn set_color(&mut self, _color: Color) -> bool {
        false
    }

    fn char_width(&self) -> isize {
        10
    }

    fn char_height(&self) -> isize {
        10
    }
}

pub trait Drawable<T>
where
    T: Copy + Clone,
{
    fn draw(&mut self, ctxt: &mut dyn RenderContext);

    fn update(&mut self, data: T);
}

pub trait Widget<T>
where
    T: Copy + Clone,
{
    fn toggle(&mut self, data: T);
    fn active(&self) -> bool;
}

/***
 * Helper functions for rendering
 */

/// reverses a buffer until end of null termination
fn reverse_buffer(buffer: &mut [u8], len: usize) {
    let mut i = 0;
    let mut j = len - 1;

    while i < j {
        buffer.swap(i, j);
        i += 1;
        j -= 1;
    }
}

pub fn to_decimal(org_value: isize, buffer: &mut [u8]) {
    if org_value == 0 {
        buffer[0] = b'0';
        buffer[1] = b'\0';
    } else {
        let mut i = 0;
        let mut value = org_value;

        buffer.fill(0);
        while value > 0 && i < buffer.len() {
            buffer[i] = (value % 10 + '0' as isize) as u8;
            i += 1;
            value /= 10;
        }

        if i < buffer.len() {
            if org_value < 0 {
                buffer[i] = b'-';
                i += 1;
            }
        }

        reverse_buffer(buffer, i);

        // terminate
        if i < buffer.len() {
            buffer[i] = 0;
        }
    }
}
