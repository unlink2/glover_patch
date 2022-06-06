use super::actions::*;
use super::cheatsmenu::*;
use super::gdbmenu::*;
use super::mainmenu::*;
use super::memory::*;
use super::renderer::GRendererContext;
use crate::actor::Actor;
use alloc::boxed::Box;
use alloc::vec::Vec;
use ultrars::clone::{CloneContext, CloneHeader};
use ultrars::frameadvance::FrameAdvance;
use ultrars::input::*;
use ultrars::math::*;
use ultrars::memory::SharedPtrCell;
use ultrars::menu::*;
use ultrars::monitor::*;
use ultrars::render::{Drawable, RenderContext, Widget};
use ultrars::timer::Timer;
use ultrars::usb::Usb;

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
    pub glover_pos: Point3<f32>,
    pub camera_pos: Point3<f32>,
    pub ball_pos: Point3<f32>,
    pub inf_lives: bool,
    pub inf_hp: bool,
    pub usb: Usb,
    pub monitor: bool,
    pub frame_advance: bool,
    pub timer: bool,
    pub infite_jump: bool,
    pub save_state: bool,
    pub load_state: bool,
    pub original_game_mode: u8,
}

impl Trigger {
    pub fn new() -> Self {
        Self {
            load_menu: true,
            menu_type: MenuType::MainMenu,
            toggle: false,
            glover_pos: Point3::new(0.0, 0.0, 0.0),
            camera_pos: Point3::new(0.0, 0.0, 0.0),
            ball_pos: Point3::new(0.0, 0.0, 0.0),
            inf_lives: false,
            inf_hp: false,
            usb: Usb::new(),
            monitor: false,
            frame_advance: false,
            timer: false,
            infite_jump: false,
            save_state: false,
            load_state: false,
            original_game_mode: 0,
        }
    }

    pub fn update(&mut self) {
        if self.inf_lives {
            unsafe { *LIVES = 5 }
        }

        if self.inf_hp {
            unsafe { *HEALTH = 3 }
        }

        if self.infite_jump {
            unsafe { *INFINITE_DOUBLE_JUMP = 0 }
        }
    }
}

pub struct InjectState<'a> {
    controller1: InputHandler,
    controller2: InputHandler,
    render_ctxt: GRendererContext<'a>,
    start_timer: u16,
    trigger: Trigger,
    menu: Box<dyn Widget<SharedPtrCell<Trigger>>>,
    timer: Timer<SharedPtrCell<Trigger>>,
    clones: CloneContext,
}

impl Default for InjectState<'_> {
    fn default() -> Self {
        // use built-in interupt enable/disable functions
        Self {
            start_timer: 0,
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2),
            render_ctxt: GRendererContext::new(),
            trigger: Trigger::new(),
            menu: Box::new(MenuFocus::Menu(InjectState::build_menu(
                MenuType::MainMenu,
                &Trigger::new(),
            ))),
            timer: Timer::new(140, 200),
            clones: CloneContext::new(0x80610000 as *mut u8, 0x80600000 as *mut CloneHeader),
        }
    }
}

impl InjectState<'_> {
    pub fn new() -> Self {
        Self::default()
    }

    /// render pushes the next frame to
    /// the framebuffer
    /// it should not do anything else!
    /// # Safety
    ///     Render needs hardware access
    pub unsafe fn render(&mut self) {
        self.menu.draw(&mut self.render_ctxt);
        self.timer.draw(&mut self.render_ctxt);
        self.render_ctxt.draw();
    }

    /// update sets up the next frame and handles the general
    /// logic
    /// # Safety
    ///     update needs to be unsafe
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
        } else if self.trigger.frame_advance {
            self.menu = MenuFocus::FrameAdvance(FrameAdvance::new(
                20,
                60,
                Entry::new("close", no_op, main_action),
            ));
            self.trigger.frame_advance = false;
        }
        self.timer.active = self.trigger.timer;

        if self.trigger.save_state {
            self.save_state(0);
            self.trigger.save_state = false;
        }
        if self.trigger.load_state {
            self.load_state(0);
            self.trigger.load_state = false;
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
            MenuFocus::FrameAdvance(_) => self.update_frame_advance(),
        }

        self.menu.update(trigger_cell);

        self.timer.set(*IN_GAME_TIME);

        self.trigger.update();
    }

    pub fn update_frame_advance(&mut self) {
        let mut trigger_cell = SharedPtrCell::new(&mut self.trigger);
        if let MenuFocus::FrameAdvance(frame_adv) = &mut self.menu {
            if self.controller1.read_button(Button::CUInput, true)
                && self.controller1.read_button(Button::LInput, false)
            {
                frame_adv.close(trigger_cell);
                unfreeze(trigger_cell);
            } else if self.controller1.read_button(Button::CDInput, true)
                && self.controller1.read_button(Button::LInput, false)
            {
                unfreeze(trigger_cell);
            } else {
                unpause();
                freeze(trigger_cell);
            }
        }
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

                if self.controller1.read_button(Button::ZInput, true) {
                    monitor.addr_input();
                } else if self.controller1.read_button(Button::StartInput, true) {
                    monitor.enter();
                } else if self.controller1.read_button(Button::AInput, true) {
                    monitor.select();
                }

                if self.controller1.read_button(Button::LInput, true) {
                    monitor.toggle_ascii();
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

    fn save_state(&mut self, _state: usize) {
        // grab all actors
        let mut current = GLOVER_ACTOR;
        loop {
            self.clones.clone(current);
            unsafe {
                current = (*current).next;
            }
            if current == GLOVER_ACTOR {
                break;
            }
        }

        // grab camera
        self.clones.clone(CAMERA);
    }

    fn load_state(&mut self, _state: usize) {
        self.clones.restore_all();
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
                    Entry::new("Save...", no_op, save_state_action),
                    Entry::new("Load...", no_op, load_state_action),
                    Entry::checkbox("Timer", no_op, timer_action, trigger.timer),
                    Entry::new("Gdb...", no_op, gdb_action),
                    Entry::new("Lives 0", no_op, lives_zero_action),
                    Entry::new("Frame advance", no_op, frame_advance_action),
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
                    Entry::checkbox("Fog", no_op, fog_cheat_action, unsafe { *FOG == 1 }),
                    Entry::checkbox("Collision", no_op, collision_cheat_action, unsafe {
                        *COLLISION_DISABLE != 0x0
                    }),
                    Entry::checkbox("CS Skip", no_op, demo_end_cheat_action, unsafe {
                        *DEMO_END_TIMER != 0
                    }),
                    Entry::checkbox("Debug Graph", no_op, debug_graph_cheat_action, unsafe {
                        *DEBUG_GRAPH == 1
                    }),
                    Entry::checkbox("Pause", no_op, disable_pause_cheat_action, unsafe {
                        *DISABLE_PAUSE_FLAG == 0
                    }),
                    Entry::checkbox(
                        "Jump",
                        no_op,
                        inf_dobule_jump_cheat_action,
                        trigger.infite_jump,
                    ),
                    Entry::new("Get ball", no_op, summon_ball_action),
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
