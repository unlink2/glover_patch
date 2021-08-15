use super::actions::*;
use super::cheatsmenu::*;
use super::gdbmenu::*;
use super::mainmenu::*;
use super::memory::*;
use super::renderer::GRendererContext;
use super::ultrars::input::*;
use super::ultrars::math::*;
use super::ultrars::memory::SharedPtrCell;
use super::ultrars::menu::*;
use super::ultrars::monitor::*;
use super::ultrars::usb::Usb;
use crate::ultrars::render::{Drawable, RenderContext, Widget};

#[derive(Copy, Clone)]
pub enum MenuType {
    MainMenu,
    CheatMenu,
    GdbMenu,
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
    pub inf_hp: bool,
    pub usb: Usb,
    pub monitor: bool,
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
            inf_hp: false,
            usb: Usb::new(),
            monitor: false,
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

pub struct InjectState<'a> {
    controller1: InputHandler,
    controller2: InputHandler,
    render_ctxt: GRendererContext<'a>,
    start_timer: u16,
    trigger: Trigger,
    menu: MenuFocus<SharedPtrCell<Trigger>>,
}

impl InjectState<'_> {
    pub fn new() -> Self {
        // use built-in interupt enable/disable functions
        Self {
            start_timer: 0,
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2),
            render_ctxt: GRendererContext::new(),
            trigger: Trigger::new(),
            menu: MenuFocus::Menu(InjectState::build_menu(MenuType::MainMenu, &Trigger::new())),
        }
    }

    /// render pushes the next frame to
    /// the framebuffer
    /// it should not do anything else!
    pub unsafe fn render(&mut self) {
        self.menu.draw(&mut self.render_ctxt);
        self.render_ctxt.draw();
    }

    /// update sets up the next frame and handles the general
    /// logic
    pub unsafe fn update(&mut self) {
        if self.start_timer < 2 {
            self.start_timer += 1;
            return;
        }

        if self.trigger.monitor {
            self.menu = MenuFocus::Monitor(Monitor::new(
                20,
                60,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, main_action),
            ));
            self.trigger.monitor = false;
        }

        if self.controller1.read_button(Button::StartInput, false)
            && self.controller1.read_button(Button::BInput, false)
        {
            self.level_select();
        }

        self.controller1.update();
        self.controller2.update();

        // update menus

        let trigger_cell = SharedPtrCell::new(&mut self.trigger);
        if self.trigger.load_menu {
            self.trigger.toggle = self.menu.active();
            self.menu = MenuFocus::Menu(InjectState::build_menu(
                self.trigger.menu_type,
                &self.trigger,
            ));
            self.trigger.load_menu = false;
        }

        if (self.controller1.read_button(Button::LInput, false)
            && self.controller1.read_button(Button::RInput, false))
            || self.trigger.toggle
        {
            self.menu.toggle(trigger_cell);
            self.trigger.toggle = false;
        }

        match &mut self.menu {
            MenuFocus::Menu(_) => self.update_menu(),
            MenuFocus::Monitor(_) => self.update_monitor(),
        }

        self.menu.update(trigger_cell);

        self.trigger.update();
    }

    pub fn update_monitor(&mut self) {
        let trigger_cell = SharedPtrCell::new(&mut self.trigger);
        if let MenuFocus::Monitor(monitor) = &mut self.menu {
            if monitor.active {
                if self.controller1.read_button(Button::BInput, true) {
                    monitor.back(trigger_cell);
                }

                if self.controller1.read_button(Button::DpadUp, true) {
                    monitor.up();
                } else if self.controller1.read_button(Button::DpadDown, true) {
                    monitor.down();
                } else if self.controller1.read_button(Button::DpadRight, true) {
                    monitor.right();
                } else if self.controller1.read_button(Button::DpadLeft, true) {
                    monitor.left();
                }

                if self.controller1.read_button(Button::CUInput, true) {
                    monitor.inc_value();
                } else if self.controller1.read_button(Button::CDInput, true) {
                    monitor.dec_value();
                }

                if self.controller1.read_button(Button::CRInput, true) {
                    monitor.inc_addr();
                } else if self.controller1.read_button(Button::CLInput, true) {
                    monitor.dec_addr();
                }
            }
        }
    }

    pub fn update_menu(&mut self) {
        let trigger_cell = SharedPtrCell::new(&mut self.trigger);
        if let MenuFocus::Menu(menu) = &mut self.menu {
            // cursor
            if menu.active {
                if self.controller1.read_button(Button::DpadUp, true) {
                    menu.dec_cursor();
                } else if self.controller1.read_button(Button::DpadDown, true) {
                    menu.inc_cursor();
                }

                if self.controller1.read_button(Button::AInput, true) {
                    menu.activate(trigger_cell);
                }

                if self.controller1.read_button(Button::BInput, true) {
                    menu.back(trigger_cell);
                }
            }
        }
    }

    unsafe fn level_select(&self) {
        *DISABLE_PAUSE_FLAG = 0x00;
        *DISABLE_INPUT_TIMER = 0x00;
        *GAME_MODE = 0x02;
        *PAUSE_FLAG = 0x00;
    }

    fn build_menu(menu_type: MenuType, trigger: &Trigger) -> Menu<SharedPtrCell<Trigger>> {
        let x = 20;
        let y = 60;
        match menu_type {
            MenuType::MainMenu => Menu::new(
                x,
                y,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, close_menu),
                Entry::new("update", no_op, update_menu),
                &[
                    Entry::new("Level Select...", no_op, level_select_action),
                    Entry::new("Monitor...", no_op, monitor_action),
                    Entry::new("Cheats...", no_op, cheats_action),
                    Entry::new("Gdb...", no_op, gdb_action),
                ],
            ),
            MenuType::CheatMenu => Menu::new(
                x,
                y,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, main_action),
                Entry::new("update", no_op, update_menu),
                &[
                    Entry::checkbox("Inf Lives", no_op, lives_cheat_action, trigger.inf_lives),
                    Entry::checkbox("Inf Health", no_op, hp_cheat_action, trigger.inf_hp),
                ],
            ),
            MenuType::GdbMenu => Menu::new(
                x,
                y,
                Entry::new("open", no_op, open_menu),
                Entry::new("close", no_op, close_menu),
                Entry::new("back", no_op, main_action),
                Entry::new("update", no_op, update_menu),
                &[Entry::checkbox(
                    "Enable Gdb...",
                    no_op,
                    enable_gdb_action,
                    trigger.usb.inited,
                )],
            ),
        }
    }
}
