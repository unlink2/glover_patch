use crate::{
    color::Color,
    font::{CHAR_H, CHAR_W},
    input::InputHandler,
    render::{Drawable, RenderContext, Widget},
};
use alloc::{boxed::Box, vec::Vec};

use super::{ActionFn, Entry, EntryFn};

pub struct Menu<T> {
    cursor: isize,
    x: isize,
    y: isize,
    pub active: bool,
    toggle_timer_max: u16,
    toggle_timer: u16,

    open_action: ActionFn<T>,
    close_action: ActionFn<T>,
    back_action: ActionFn<T>,
    update_action: ActionFn<T>,
    entries: Vec<Box<dyn Widget<T>>>,
}

impl<T> Menu<T> {
    pub fn new(
        x: isize,
        y: isize,
        open_action: ActionFn<T>,
        close_action: ActionFn<T>,
        back_action: ActionFn<T>,
        update_action: ActionFn<T>,
        entries: Vec<Box<dyn Widget<T>>>,
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
            entries,
        }
    }

    pub fn inc_cursor(&mut self) {
        self.cursor += 1;
        if self.cursor >= self.entries.len() as isize {
            self.cursor = 0;
        }
    }

    pub fn dec_cursor(&mut self) {
        self.cursor -= 1;
        if self.cursor < 0 {
            self.cursor = self.entries.len() as isize - 1;
        }
    }

    pub fn activate(&mut self, data: &mut T) {
        if !self.active {
            return;
        }

        self.entries[self.cursor as usize].activate(data);
    }

    pub fn open(&mut self, data: &mut T) {
        self.active = true;
        (self.open_action)(data);
    }

    pub fn close(&mut self, data: &mut T) {
        self.active = false;
        (self.close_action)(data);
    }

    pub fn back(&mut self, data: &mut T) {
        self.toggle_timer = 0;
        (self.back_action)(data);
    }
}

impl<T> Drawable<T> for Menu<T> {
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
            entry.position(start_x + CHAR_W as isize + 2_isize, start_y);
            entry.draw(ctxt);
            start_y += CHAR_H as isize + 2;

            counter += 1;
        }
    }

    fn update(&mut self, data: &mut T, input: &InputHandler) {
        if self.toggle_timer > 0 {
            self.toggle_timer -= 1;
        }
        if !self.active {
            return;
        }

        (self.update_action)(data);

        for entry in &mut self.entries {
            if !entry.active() {
                continue;
            }

            entry.update(data, input);
        }
    }
}

impl<T> Widget<T> for Menu<T> {
    fn toggle(&mut self, data: &mut T) {
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

    fn activate(&mut self, _data: &mut T) {}

    fn position(&mut self, x: isize, y: isize) {}
}
