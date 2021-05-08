extern crate ultrars;
use self::ultrars::input::*;
use super::memory::*;

pub struct InjectState {
    controller1: InputHandler,
    controller2: InputHandler
}

impl InjectState {
    pub fn new() -> Self {
        Self {
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2)
        }
    }

    pub unsafe fn update(&mut self) {
        self.controller1.update();
        self.controller2.update();
		if self.controller1.read_button(Button::StartInput, false)
			&& self.controller1.read_button(Button::BInput, false) {
			self.level_select();
		}
    }

    unsafe fn level_select(&self) {
		*DISABLE_PAUSE_FLAG = 0x00;
		*DISABLE_INPUT_TIMER = 0x00;
		*GAME_MODE = 0x02;
	}
}
