use crate::{
    color::Color,
    font::{CHAR_H, CHAR_W},
    input::InputHandler,
    render::{Drawable, RenderContext, Widget},
};
use alloc::vec::Vec;

use super::EntryTypes;

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

    fn update(&mut self, data: T, input: &InputHandler) {
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
