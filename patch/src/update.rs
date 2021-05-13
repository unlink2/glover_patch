extern crate ultrars;
use self::ultrars::input::*;
use self::ultrars::rdp::*;
use self::ultrars::font::*;
use self::ultrars::menu::*;
use super::memory::*;
use super::ultrars::memory::SharedPtrCell;

fn open_menu(entry: &mut Entry<SharedPtrCell<Trigger>>, trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 1;
    }
    return None;
}

fn close_menu(entry: &mut Entry<SharedPtrCell<Trigger>>, trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 0;
    }
    return None;
}

fn update_menu(entry: &mut Entry<SharedPtrCell<Trigger>>, trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 1;
    }
    return None;
}

#[derive(Copy, Clone)]
pub struct Trigger {
    pub menu_active: bool
}

impl Trigger {
    pub fn new() -> Self {
        Self {
            menu_active: false
        }
    }
}

pub struct InjectState {
    controller1: InputHandler,
    controller2: InputHandler,
    rdp_ctxt: RdpFontRendererContext,
    font: Font,
    start_timer: u16,
    trigger: Trigger,
    menu: Menu<SharedPtrCell<Trigger>>,
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
                196608),
            font: Font::new(&FONT8X8_BASIC, 0x80525C30 as *mut u16, 0x000F, 0xFFFF),
            trigger: Trigger::new(),
            menu: Menu::new(10, 10,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, no_op),
                Entry::new("update", no_op, update_menu),
                &[Entry::new("Test", no_op, no_op),
                  Entry::new("Test", no_op, no_op),
                  Entry::new("Test", no_op, no_op)])
        }
    }

    pub unsafe fn update(&mut self) {
        if self.start_timer < 2 {
            self.start_timer += 1;
            return;
        }

        if self.controller1.read_button(Button::StartInput, false)
			&& self.controller1.read_button(Button::BInput, false) {
			self.level_select();
		}

        self.controller1.update();
        self.controller2.update();

        self.update_menu();
    }

    pub fn update_menu(&mut self) {
        let trigger_cell = SharedPtrCell::new(&mut self.trigger);

        if self.controller1.read_button(Button::LInput, false)
            && self.controller1.read_button(Button::RInput, false) {
            self.menu.toggle(trigger_cell);
        }

        // cursor
        if self.menu.active {
            if self.controller1.read_button(Button::DpadUp, true) {
                self.menu.dec_cursor();
            } else if self.controller1.read_button(Button::DpadDown, true) {
                self.menu.inc_cursor();
            }

            if self.controller1.read_button(Button::AInput, true) {
                self.menu.activate(trigger_cell);
            }
        }

        self.menu.update(trigger_cell);

        self.rdp_ctxt.update();

        self.menu.draw(&mut self.rdp_ctxt, &mut self.font);
    }

    unsafe fn level_select(&self) {
		*DISABLE_PAUSE_FLAG = 0x00;
		*DISABLE_INPUT_TIMER = 0x00;
		*GAME_MODE = 0x02;
	}
}
