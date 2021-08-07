use super::ultrars::render::RenderContext;
use super::ultrars::font::GenericFont;
use super::memory::{PUTS_Y_POS, PUTS_X_POS, PUTS_XY, PutsXy};

pub struct GRendererContext;

/// TODO render context for internal font renderer
impl RenderContext for GRendererContext {
    fn update(&mut self) {
    }

    fn puts(&mut self, s: &str, x: i32, y: i32, font: &dyn GenericFont) {
    }

    fn cputs(&mut self, s: &[char], x: i32, y: i32, font: &dyn GenericFont) {
    }

    fn draw_char(&mut self, c: char, x: i32, y: i32, font: &dyn GenericFont) {
    }
}
