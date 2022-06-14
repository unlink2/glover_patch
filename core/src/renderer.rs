use alloc::boxed::Box;
use core::ffi::c_void;
use core::ptr::{self, null};
use ultrars::color::Color;
use ultrars::render::RenderContext;

#[repr(C)]
struct TextNode {
    next: *const TextNode,
    prev: *const TextNode,
    text: [u8; 0x40],
    pos_x: isize,
    pos_y: isize,
    scale_x: f32,
    scale_y: f32,
    color: Color,
    unkown0: u32,
    font: *const c_void, // TODO make struct
    unknown1: [u32; 3],
}

impl Default for TextNode {
    fn default() -> Self {
        Self {
            next: ptr::null(),
            prev: ptr::null(),
            text: [0; 0x40],
            pos_x: Default::default(),
            pos_y: Default::default(),
            scale_x: Default::default(),
            scale_y: Default::default(),
            color: Color {
                red: 0,
                green: 0,
                blue: 0,
                alpha: 0,
            },
            unkown0: Default::default(),
            font: ptr::null(),
            unknown1: Default::default(),
        }
    }
}

impl TextNode {
    pub fn new(pos_x: isize, pos_y: isize, scale_x: f32, scale_y: f32, color: Color) -> Self {
        Self {
            next: null() as *const TextNode,
            prev: null() as *const TextNode,
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

const CAP: usize = 32;

pub struct GRendererContext {
    buffer: Box<[TextNode; CAP]>,
    current: usize,
    next_color: Color,
}

impl GRendererContext {
    pub fn new() -> Self {
        let buffer: [TextNode; CAP] = Default::default();
        let mut s = Self {
            current: 0,
            buffer: Box::new(buffer),
            next_color: Color::new(0xFF, 0xFF, 0xFF, 0xFE),
        };

        s.link();
        s
    }

    fn insert(&mut self, x: isize, y: isize) {
        let mut b = &mut self.buffer[self.current];
        b.pos_x = x;
        b.pos_y = y;
    }

    fn link(&mut self) {
        unsafe {
            let link = core::mem::transmute::<*const c_void, fn(*const TextNode, u8)>(
                super::memory::INSERT_TEXT_LL,
            );
            for b in self.buffer.iter_mut() {
                *b = TextNode::new(0, 0, 0.75, 0.75, Color::new(0xFF, 0xFF, 0xFF, 0xFE));
                (link)(b, 1);
                b.font = 0x801ED338 as *const c_void;
            }
        }
    }

    fn set_color(&mut self) {
        self.buffer[self.current].color = self.next_color;
        self.next_color = Color::new(0xFF, 0xFF, 0xFF, 0xFE);
    }
}

impl Default for GRendererContext {
    fn default() -> Self {
        Self::new()
    }
}

/// TODO render context for internal font renderer
impl RenderContext for GRendererContext {
    fn draw(&mut self) {
        if self.buffer[self.current].next.is_null() {
            self.link();
        }
        for i in self.current..self.buffer.len() {
            self.buffer[i].text[0] = b'\0';
        }
        self.current = 0;
    }

    fn puts(&mut self, s: &str, x: isize, y: isize) {
        self.insert(x, y);
        unsafe {
            ultrars::memory::umemset(
                self.buffer[self.current].text.as_ptr() as *mut u8,
                0,
                self.buffer[self.current].text.len(),
            );
        }
        for (i, c) in s.as_bytes().iter().enumerate() {
            self.buffer[self.current].text[i] = self.convert(*c);
        }
        self.set_color();

        self.current += 1;
    }

    fn cputs(&mut self, s: &[char], x: isize, y: isize) {
        self.insert(x, y);
        unsafe {
            ultrars::memory::umemset(
                self.buffer[self.current].text.as_ptr() as *mut u8,
                0,
                self.buffer[self.current].text.len(),
            );
        }

        for (i, c) in s.iter().enumerate() {
            self.buffer[self.current].text[i] = self.convert(*c as u8);
        }
        self.set_color();

        self.current += 1;
    }

    fn putsu8(&mut self, s: &[u8], x: isize, y: isize) {
        self.insert(x, y);
        unsafe {
            ultrars::memory::umemset(
                self.buffer[self.current].text.as_ptr() as *mut u8,
                0,
                self.buffer[self.current].text.len(),
            );
        }

        for (i, c) in s.iter().enumerate() {
            if *c == b'\0' {
                break;
            } else {
                self.buffer[self.current].text[i] = self.convert(*c as u8);
            }
        }
        self.set_color();

        self.current += 1;
    }

    fn set_color(&mut self, color: Color) -> bool {
        self.next_color = color;
        true
    }

    // font does not have lower case letters
    fn convert(&self, c: u8) -> u8 {
        if (b'a'..=b'z').contains(&c) {
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
