use crate::input::InputHandler;
use crate::menu::ActionFn;
use crate::render::Drawable;
use crate::render::RenderContext;
use crate::render::Widget;

pub struct FrameAdvance<T> {
    x: isize,
    y: isize,
    toggle_timer_max: u16,
    toggle_timer: u16,
    active: bool,
    close_action: ActionFn<T>,
}

impl<T> FrameAdvance<T> {
    pub fn new(x: isize, y: isize, close_action: ActionFn<T>) -> Self {
        Self {
            active: true,
            toggle_timer_max: 10,
            toggle_timer: 0,
            close_action,
            x,
            y,
        }
    }
    pub fn open(&mut self, data: &mut T) {
        self.active = true;
    }

    pub fn close(&mut self, data: &mut T) {
        (self.close_action)(data);
        self.active = false;
    }
}

impl<T> Widget<T> for FrameAdvance<T> {
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

    fn activate(&mut self, data: &mut T) {}

    fn position(&mut self, x: isize, y: isize) {}
}

impl<T> Drawable<T> for FrameAdvance<T> {
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        if !self.active {
            return;
        }
        ctxt.puts("Frame Advance", self.x, self.y);
    }

    fn update(&mut self, data: &mut T, input: &InputHandler) {
        if self.toggle_timer > 0 {
            self.toggle_timer -= 1;
        }
        if !self.active {
            return;
        }
    }
}
