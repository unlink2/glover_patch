use crate::{
    frameadvance::FrameAdvance,
    monitor::Monitor,
    render::{Drawable, RenderContext, Widget},
};

use super::menu::Menu;

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
