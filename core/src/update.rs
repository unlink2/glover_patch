extern crate ultrars;
use self::ultrars::input::*;
use self::ultrars::rdp::*;
use self::ultrars::font::*;
use self::ultrars::menu::*;
use self::ultrars::math::*;
use self::ultrars::interrupt::IntFn;
use super::actor::*;
use super::camera::*;
use super::memory::*;
use super::mainmenu::*;
use super::ultrars::memory::SharedPtrCell;
use super::memory::{ENABLE_INTERRUPT, DISABLE_INTERRUPT};
use super::actions::*;
use crate::ultrars::render::RenderContext;
use core::ffi::c_void;


#[derive(Copy, Clone)]
pub enum MenuType {
    MainMenu,
    CheatMenu,
}

#[derive(Copy, Clone)]
pub struct Trigger {
    pub load_menu: bool,
    pub menu_type: MenuType,
    pub toggle: bool,
    pub glover_pos: Vector3<f32>,
    pub camera_pos: Vector3<f32>,
    pub ball_pos: Vector3<f32>,
    pub inf_lives: bool,
    pub inf_hp: bool
}

impl Trigger {
    pub fn new() -> Self {
        Self {
            load_menu: true,
            menu_type: MenuType::MainMenu,
            toggle: false,
            glover_pos: Vector3::new(0.0, 0.0, 0.0),
            camera_pos: Vector3::new(0.0, 0.0, 0.0),
            ball_pos: Vector3::new(0.0, 0.0, 0.0),
            inf_lives: false,
            inf_hp: false
        }
    }

    pub fn update(&mut self) {
        if self.inf_lives {
            unsafe { *LIVES = 5 }
        }

        if self.inf_hp {
            unsafe { *HEALTH = 3 }
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
        // use built-in interupt enable/disable functions
        let di = unsafe { core::mem::transmute::<*const c_void, IntFn>(DISABLE_INTERRUPT) };
        let ei = unsafe { core::mem::transmute::<*const c_void, IntFn>(DISABLE_INTERRUPT) };

        Self {
            start_timer: 0,
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2),
            // TODO hard coded ptr is bad ok?
            rdp_ctxt: RdpFontRendererContext::new(
                0x80529D40 as *mut u32,
                196608,
                di,
                ei
            ),
            font: Font::new(&FONT8X8_BASIC, 0x80525C30 as *mut u16, 0x000F, 0xFFFF),
            trigger: Trigger::new(),
            menu: InjectState::build_menu(MenuType::MainMenu, &Trigger::new())
        }
    }

    /// render pushes the next frame to
    /// the framebuffer
    /// it should not do anything else!
    pub unsafe fn render(&mut self) {
        self.rdp_ctxt.update();
    }

    /// update sets up the next frame and handles the general
    /// logic
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

        self.trigger.update();
    }

    pub fn update_menu(&mut self) {
        let trigger_cell = SharedPtrCell::new(&mut self.trigger);
        if self.trigger.load_menu {
            self.trigger.toggle = self.menu.active;
            self.menu = InjectState::build_menu(self.trigger.menu_type, &self.trigger);
            self.trigger.load_menu = false;
        }

        if (self.controller1.read_button(Button::LInput, false)
            && self.controller1.read_button(Button::RInput, false))
            || self.trigger.toggle {
            self.menu.toggle(trigger_cell);
            self.trigger.toggle = false;
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

            if self.controller1.read_button(Button::BInput, true) {
                self.menu.back(trigger_cell);
            }
        }

        self.menu.update(trigger_cell);

        self.menu.draw(&mut self.rdp_ctxt, &mut self.font);
    }

    unsafe fn level_select(&self) {
		*DISABLE_PAUSE_FLAG = 0x00;
		*DISABLE_INPUT_TIMER = 0x00;
		*GAME_MODE = 0x02;
        *PAUSE_FLAG = 0x00;
	}

    fn build_menu(menu_type: MenuType, trigger: &Trigger) -> Menu<SharedPtrCell<Trigger>> {
        match menu_type {
            MenuType::MainMenu => Menu::new(10, 10,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, close_menu),
                Entry::new("update", no_op, update_menu),

                &[Entry::new("Level Select...", no_op, level_select_action),
                  Entry::new("Cheats...", no_op, cheats_action)]),
            MenuType::CheatMenu => Menu::new(10, 10,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, main_action),
                Entry::new("update", no_op, update_menu),
                &[Entry::checkbox("Inf Lives", no_op, lives_cheat_action, trigger.inf_lives),
                Entry::checkbox("Inf Health", no_op, hp_cheat_action, trigger.inf_hp)])
        }
    }
}
