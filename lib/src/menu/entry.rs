use core::marker::PhantomData;

use alloc::boxed::Box;
use alloc::string::String;
use alloc::string::ToString;

use crate::input::InputHandler;
use crate::render::Drawable;

use crate::render::{RenderContext, Widget};

/**
 * An action function that returns either None
 * or a usize value
 */
pub type EntryFn<T> = fn(entry: &mut Entry<T>, data: &mut T) -> Option<usize>;
pub type ActionFn<T> = fn(data: &mut T) -> Option<usize>;

#[derive(Clone)]
pub struct Entry<T> {
    pub title: String,
    pub active: bool,
    pub update: EntryFn<T>,
    pub action: EntryFn<T>,
    x: isize,
    y: isize,
}

pub fn no_op<T>(_entry: &mut Entry<T>, _data: &mut T) -> Option<usize> {
    None
}

pub fn no_op_action<T>(_data: &mut T) -> Option<usize> {
    None
}

impl<T> Entry<T> {
    pub fn empty() -> Self {
        Self {
            title: "".to_string(),
            update: no_op,
            action: no_op,
            active: false,
            x: 0,
            y: 0,
        }
    }

    #[allow(clippy::new_ret_no_self)]
    pub fn new(title: &str, update: EntryFn<T>, action: EntryFn<T>) -> Self {
        Self {
            title: title.to_string(),
            update,
            action,
            active: true,
            x: 0,
            y: 0,
        }
    }

    pub fn checkbox(title: &str, update: EntryFn<T>, action: EntryFn<T>, value: bool) -> Self {
        let mut title_ca = if value {
            "[x]".to_string()
        } else {
            "[ ]".to_string()
        };
        title_ca.push_str(title);

        Self {
            title: title_ca,
            update,
            action,
            active: true,
            x: 0,
            y: 0,
        }
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

    pub fn call_update(&mut self, data: &mut T) {
        (self.update)(self, data);
    }
}

impl<T> Drawable<T> for Entry<T> {
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        ctxt.puts(&self.title, self.x, self.y);
    }

    fn update(&mut self, data: &mut T, _input: &InputHandler) {
        self.call_update(data)
    }
}

impl<T> Widget<T> for Entry<T> {
    fn toggle(&mut self, _data: &mut T) {}

    fn activate(&mut self, data: &mut T) {
        (self.action)(self, data);
    }

    fn position(&mut self, x: isize, y: isize) {
        self.x = x;
        self.y = y;
    }

    fn active(&self) -> bool {
        self.active
    }
}
