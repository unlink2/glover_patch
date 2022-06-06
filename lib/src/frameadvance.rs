use crate::input::InputHandler;
use crate::menu::EntryTypes;
use crate::render::Drawable;
use crate::render::RenderContext;
use crate::render::Widget;

pub struct FrameAdvance<T>
where
    T: Copy + Clone,
{
    x: isize,
    y: isize,
    toggle_timer_max: u16,
    toggle_timer: u16,
    active: bool,
    close_action: EntryTypes<T>,
}

impl<T> FrameAdvance<T>
where
    T: Copy + Clone,
{
    pub fn new(x: isize, y: isize, close_action: EntryTypes<T>) -> Self {
        Self {
            active: true,
            toggle_timer_max: 10,
            toggle_timer: 0,
            close_action,
            x,
            y,
        }
    }
    pub fn open(&mut self, data: T) {
        self.active = true;
    }

    pub fn close(&mut self, data: T) {
        self.close_action.activate(data);
        self.active = false;
    }
}

impl<T> Widget<T> for FrameAdvance<T>
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

impl<T> Drawable<T> for FrameAdvance<T>
where
    T: Copy + Clone,
{
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        if !self.active {
            return;
        }
        ctxt.puts("Frame Advance", self.x, self.y);
    }

    fn update(&mut self, data: T, input: &InputHandler) {
        if self.toggle_timer > 0 {
            self.toggle_timer -= 1;
        }
        if !self.active {
            return;
        }
    }
}
