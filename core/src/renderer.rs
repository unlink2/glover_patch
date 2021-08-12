use super::ultrars::render::RenderContext;
use super::ultrars::font::GenericFont;
use super::ultrars::color::Color;
use core::ffi::c_void;
use super::memory::TEXT_LL_HEAD;

#[repr(C)]
struct TextDrawObject {
    next: *const TextDrawObject,
    prev: *const TextDrawObject,
    text: [u8; 0x40],
    pos_x: i32,
    pos_y: i32,
    scale_x: f32,
    scale_y: f32,
    color: Color,
    unkown0: u32,
    font: *const c_void, // TODO make struct
    unknown1: [u32; 3],
}

impl TextDrawObject {
    pub fn new(pos_x: i32, pos_y: i32, scale_x: f32, scale_y: f32, color: Color) -> Self {
        Self {
            next: 0 as *const TextDrawObject,
            prev: 0 as *const TextDrawObject,
            text: [b'\0'; 0x40],
            pos_x,
            pos_y,
            scale_x,
            scale_y,
            color,
            unkown0: 0,
            font: 0x801ED338 as *const c_void,
            unknown1: [0; 3],
        }
    }
}


pub struct GRendererContext<'a> {
    buffer: &'a mut [TextDrawObject],
    current: usize,
}

impl GRendererContext<'_> {
    pub fn new() -> Self {
        let buffer = unsafe { core::slice::from_raw_parts_mut(0x80550000 as *mut TextDrawObject, 100) };
        Self {
            current: 0,
            buffer,
        }
    }

    fn insert(&mut self, x: i32, y: i32) {
        self.buffer[self.current] = TextDrawObject::new(x, y, 1.0, 1.0, Color::new(0xFF, 0xFF, 0xFF, 0xFE));
        self.link();
    }

    fn link(&mut self) {
        if self.current > 0 {
            self.buffer[self.current].prev = &self.buffer[self.current] as *const TextDrawObject;
            self.buffer[self.current-1].next = &self.buffer[self.current] as *const TextDrawObject;
        }
    }

    // font does not have lower case letters
    fn adjust_char(c: u8) -> u8 {
        if c >= b'a' && c <= b'z' {
            c-32
        } else {
            c
        }
    }
}

/// TODO render context for internal font renderer
impl RenderContext for GRendererContext<'_> {
    fn draw(&mut self) {
        // link first and last item to actual list
        let head = TEXT_LL_HEAD as *mut TextDrawObject;

        if self.current > 0 {
            unsafe {
                self.buffer[self.current-1].next = (*(*head).next).next;
                self.buffer[0].prev = (*head).next;

                // (*head).next = &self.buffer[0] as *const TextDrawObject;
            }

            self.current = 0;
        }
    }

    fn puts(&mut self, s: &str, x: i32, y: i32) {
        self.insert(x, y);
        for (i, c) in s.as_bytes().iter().enumerate() {
            self.buffer[self.current].text[i] = Self::adjust_char(*c);
        }

        self.current += 1;
    }

    fn cputs(&mut self, s: &[char], x: i32, y: i32) {
        self.insert(x, y);
        for (i, c) in s.iter().enumerate() {
            self.buffer[self.current].text[i] = Self::adjust_char(*c as u8);
        }

        self.current += 1;
    }
}
