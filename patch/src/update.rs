extern crate ultrars;
use self::ultrars::input::*;
use self::ultrars::rdp::*;
use self::ultrars::font::*;
use super::memory::*;

pub struct InjectState {
    controller1: InputHandler,
    controller2: InputHandler,
    rdp_ctxt: RdpFontRendererContext,
    font: Font,
    start_timer: u16
}

impl InjectState {
    pub fn new() -> Self {
        Self {
            start_timer: 0,
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2),
            // TODO hard coded ptr is bad ok?
            rdp_ctxt: RdpFontRendererContext::new(
                0x80529D40 as *mut u32,
                786432),
            font: Font::new(&FONT8X8_BASIC, 0x80525C30 as *mut u16, 0x000F, 0xFFFF)
        }
    }

    pub unsafe fn update(&mut self) {
        if self.start_timer < 2 {
            self.start_timer += 1;
            return;
        }

        self.controller1.update();
        self.controller2.update();
		if self.controller1.read_button(Button::StartInput, false)
			&& self.controller1.read_button(Button::BInput, false) {
			self.level_select();
		}
        self.rdp_ctxt.update();

		// try rendering something :o
        self.rdp_ctxt.puts("Hello", 20, 20, &self.font);
    }

    unsafe fn level_select(&self) {
		*DISABLE_PAUSE_FLAG = 0x00;
		*DISABLE_INPUT_TIMER = 0x00;
		*GAME_MODE = 0x02;
	}
}
