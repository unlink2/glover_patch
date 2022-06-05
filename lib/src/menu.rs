use alloc::string::String;
use alloc::string::ToString;
use alloc::vec::Vec;

use crate::color::Color;
use crate::monitor::Monitor;
use crate::render::Drawable;

use super::font::*;
use super::math::*;
use super::render::{RenderContext, Widget};
use crate::frameadvance::FrameAdvance;

pub const MAX_ENTRIES: usize = 15;
pub const MAX_TITLE_LEN: usize = 15;

/**
 * This is a simple windowing system
 * It holds any menu-like object for easy switching
 */
pub enum MenuFocus<T>
where
    T: Copy + Clone,
{
    Menu(Menu<T>),
    Monitor(Monitor<T>),
    FrameAdvance(FrameAdvance<T>),
}

impl<T> Widget<T> for MenuFocus<T>
where
    T: Copy + Clone,
{
    fn toggle(&mut self, data: T) {
        match self {
            Self::Menu(m) => m.toggle(data),
            Self::Monitor(m) => m.toggle(data),
            Self::FrameAdvance(m) => m.toggle(data),
        }
    }

    fn active(&self) -> bool {
        match self {
            Self::Menu(m) => m.active(),
            Self::Monitor(m) => m.active(),
            Self::FrameAdvance(m) => m.active(),
        }
    }
}

impl<T> Drawable<T> for MenuFocus<T>
where
    T: Copy + Clone,
{
    fn update(&mut self, data: T) {
        match self {
            Self::Menu(m) => m.update(data),
            Self::Monitor(m) => m.update(data),
            Self::FrameAdvance(m) => m.update(data),
        }
    }

    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        match self {
            Self::Menu(m) => m.draw(ctxt),
            Self::Monitor(m) => m.draw(ctxt),
            Self::FrameAdvance(m) => m.draw(ctxt),
        }
    }
}

#[derive(Clone)]
pub enum EntryTypes<T>
where
    T: Copy + Clone,
{
    Button(Entry<T>),
}

impl<T> EntryTypes<T>
where
    T: Copy + Clone,
{
    pub fn activate(&mut self, data: T) {
        match self {
            EntryTypes::Button(b) => b.activate(data),
        }
    }

    pub fn active(&self) -> bool {
        match self {
            EntryTypes::Button(b) => b.active,
        }
    }

    pub fn draw(&mut self, ctxt: &mut dyn RenderContext, x: isize, y: isize) {
        match self {
            EntryTypes::Button(b) => b.draw(ctxt, x, y),
        }
    }

    pub fn call_update(&mut self, data: T) {
        match self {
            EntryTypes::Button(b) => b.call_update(data),
        }
    }
}

/**
 * An action function that returns either None
 * or a usize value
 */
pub type EntryFn<T> = fn(entry: &mut Entry<T>, data: T) -> Option<usize>;

#[derive(Clone)]
pub struct Entry<T>
where
    T: Copy + Clone,
{
    pub title: String,
    pub active: bool,
    pub update: EntryFn<T>,
    pub action: EntryFn<T>,
}

pub fn no_op<T: Copy + Clone>(_entry: &mut Entry<T>, _data: T) -> Option<usize> {
    None
}

impl<T> Entry<T>
where
    T: Copy + Clone,
{
    pub fn empty() -> EntryTypes<T> {
        EntryTypes::Button(Self {
            title: "".to_string(),
            update: no_op,
            action: no_op,
            active: false,
        })
    }

    #[allow(clippy::new_ret_no_self)]
    pub fn new(title: &str, update: EntryFn<T>, action: EntryFn<T>) -> EntryTypes<T> {
        EntryTypes::Button(Self {
            title: title.to_string(),
            update,
            action,
            active: true,
        })
    }

    pub fn checkbox(
        title: &str,
        update: EntryFn<T>,
        action: EntryFn<T>,
        value: bool,
    ) -> EntryTypes<T> {
        let mut title_ca = if value {
            "[x]".to_string()
        } else {
            "[ ]".to_string()
        };
        title_ca.push_str(title);

        EntryTypes::Button(Self {
            title: title_ca,
            update,
            action,
            active: true,
        })
    }

    pub fn set_checkbox(&mut self, value: bool) {
        if value {
            self.title.replace_range(0..3, "[x]");
        } else {
            self.title.replace_range(0..3, "[ ]");
        }
    }

    pub fn set_title(&mut self, title: &str) {
        self.title = title.to_string();
    }

    pub fn draw(&mut self, ctxt: &mut dyn RenderContext, x: isize, y: isize) {
        ctxt.puts(&self.title, x, y);
    }

    pub fn call_update(&mut self, data: T) {
        (self.update)(self, data);
    }

    pub fn activate(&mut self, data: T) {
        (self.action)(self, data);
    }
}

pub struct Menu<T>
where
    T: Copy + Clone,
{
    cursor: isize,
    x: isize,
    y: isize,
    pub active: bool,
    toggle_timer_max: u16,
    toggle_timer: u16,

    open_action: EntryTypes<T>,
    close_action: EntryTypes<T>,
    back_action: EntryTypes<T>,
    update_action: EntryTypes<T>,
    entries: Vec<EntryTypes<T>>,
    active_entries: usize,
}

impl<T> Menu<T>
where
    T: Copy + Clone,
{
    pub fn new(
        x: isize,
        y: isize,
        open_action: EntryTypes<T>,
        close_action: EntryTypes<T>,
        back_action: EntryTypes<T>,
        update_action: EntryTypes<T>,
        entries: &[EntryTypes<T>],
    ) -> Self {
        Self {
            cursor: 0,
            active: false,
            toggle_timer_max: 10,
            toggle_timer: 0,
            open_action,
            close_action,
            back_action,
            update_action,
            x,
            y,
            entries: entries.to_vec(),
            active_entries: entries.len(),
        }
    }

    pub fn inc_cursor(&mut self) {
        self.cursor += 1;
        if self.cursor >= self.active_entries as isize {
            self.cursor = 0;
        }
    }

    pub fn dec_cursor(&mut self) {
        self.cursor -= 1;
        if self.cursor < 0 {
            self.cursor = self.active_entries as isize - 1;
        }
    }

    pub fn activate(&mut self, data: T) {
        if !self.active {
            return;
        }

        self.entries[self.cursor as usize].activate(data);
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
        self.toggle_timer = 0;
        self.back_action.activate(data);
    }
}

impl<T> Drawable<T> for Menu<T>
where
    T: Copy + Clone,
{
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        if !self.active {
            return;
        }

        let start_x = self.x;
        let mut start_y = self.y;

        let mut counter: isize = 0;
        for entry in &mut self.entries {
            if !entry.active() {
                continue;
            }

            if self.cursor == counter && !ctxt.set_color(Color::new(0xFF, 0x00, 0x00, 0xFF)) {
                ctxt.puts(">", start_x, start_y);
            }
            entry.draw(ctxt, start_x + CHAR_W as isize + 2_isize, start_y);
            start_y += CHAR_H as isize + 2;

            counter += 1;
        }
    }

    fn update(&mut self, data: T) {
        if self.toggle_timer > 0 {
            self.toggle_timer -= 1;
        }
        if !self.active {
            return;
        }

        self.update_action.activate(data);

        for entry in &mut self.entries {
            if !entry.active() {
                continue;
            }

            entry.call_update(data);
        }
    }
}

impl<T> Widget<T> for Menu<T>
where
    T: Copy + Clone,
{
    fn toggle(&mut self, data: T) {
        if self.toggle_timer > 0 {
            return;
        }

        self.toggle_timer = self.toggle_timer_max;
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
