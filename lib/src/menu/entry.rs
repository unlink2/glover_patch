use alloc::string::String;
use alloc::string::ToString;

use crate::render::Drawable;

use crate::render::{RenderContext, Widget};

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
