use super::memory::TEXT_LL_HEAD;
use super::ultrars::color::Color;
use super::ultrars::render::RenderContext;
use core::ffi::c_void;
use core::ptr::null;

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
            next: null() as *const TextDrawObject,
            prev: null() as *const TextDrawObject,
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
        let buffer =
            unsafe { core::slice::from_raw_parts_mut(0x80550000 as *mut TextDrawObject, 100) };
        let mut s = Self { current: 0, buffer };

        s.link();
        s
    }

    fn insert(&mut self, x: i32, y: i32) {
        let mut b = &mut self.buffer[self.current];
        b.pos_x = x;
        b.pos_y = y;
    }

    fn link(&mut self) {
        unsafe {
            let link = core::mem::transmute::<*const c_void, fn(*const TextDrawObject, u8)>(
                super::memory::INSERT_TEXT_LL,
            );
            for b in self.buffer.iter_mut() {
                *b = TextDrawObject::new(0, 0, 0.75, 0.75, Color::new(0xFF, 0xFF, 0xFF, 0xFE));
                (link)(b, 1);
                b.font = 0x801ED338 as *const c_void;
            }
        }
    }

    // font does not have lower case letters
    fn adjust_char(c: u8) -> u8 {
        if c >= b'a' && c <= b'z' {
            c - 32
        } else {
            match c {
                b'>' => b'C',
                b']' => b' ',
                b'[' => b' ',
                _ => c,
            }
        }
    }
}

/// TODO render context for internal font renderer
impl RenderContext for GRendererContext<'_> {
    fn draw(&mut self) {
        if self.buffer[self.current].next.is_null() {
            self.link();
        }
        for i in self.current..self.buffer.len() {
            self.buffer[i].text[0] = b'\0';
        }
        self.current = 0;
    }

    fn puts(&mut self, s: &str, x: i32, y: i32) {
        self.insert(x, y);
        unsafe {
            super::ultrars::memory::umemset(
                self.buffer[self.current].text.as_ptr() as *mut u8,
                0,
                self.buffer[self.current].text.len(),
            );
        }
        for (i, c) in s.as_bytes().iter().enumerate() {
            self.buffer[self.current].text[i] = Self::adjust_char(*c);
        }

        self.current += 1;
    }

    fn cputs(&mut self, s: &[char], x: i32, y: i32) {
        self.insert(x, y);
        unsafe {
            super::ultrars::memory::umemset(
                self.buffer[self.current].text.as_ptr() as *mut u8,
                0,
                self.buffer[self.current].text.len(),
            );
        }

        for (i, c) in s.iter().enumerate() {
            self.buffer[self.current].text[i] = Self::adjust_char(*c as u8);
        }

        self.current += 1;
    }
}
