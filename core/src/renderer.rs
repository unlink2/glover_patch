use super::ultrars::render::RenderContext;
use super::ultrars::font::GenericFont;

pub struct GRendererContext;

/// TODO render context for internal font renderer
impl RenderContext for GRendererContext {
    fn update(&mut self) {
    }

    fn puts(&mut self, _s: &str, _x: i32, _y: i32) {
    }

    fn cputs(&mut self, _s: &[char], _x: i32, _y: i32) {
    }

    fn draw_char(&mut self, _c: char, _x: i32, _y: i32) {
    }
}
