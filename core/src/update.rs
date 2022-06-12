use core::cell::RefCell;

use super::actions::*;
use super::cheatsmenu::*;
use super::gdbmenu::*;
use super::mainmenu::*;
use super::memory::*;
use super::renderer::GRendererContext;
use crate::actor::Actor;
use alloc::boxed::Box;
use alloc::rc::Rc;
use alloc::vec;
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

pub struct InjectState {
    controller1: InputHandler,
    controller2: InputHandler,
    render_ctxt: GRendererContext,
    start_timer: u16,
    menu: Rc<RefCell<dyn Widget<InjectState>>>,
    timer: Timer<InjectState>,
    clones: CloneContext,
}

impl Default for InjectState {
    fn default() -> Self {
        // use built-in interupt enable/disable functions
        Self {
            start_timer: 0,
            controller1: InputHandler::new(CONTROLLER1),
            controller2: InputHandler::new(CONTROLLER2),
            render_ctxt: GRendererContext::new(),
            menu: Rc::new(RefCell::new(Menu::new(
                0,
                0,
                no_op_action,
                no_op_action,
                no_op_action,
                no_op_action,
                vec![Box::new(Entry::new("Test Entry", no_op, no_op))],
            ))),
            timer: Timer::new(140, 200),
            clones: CloneContext::new(0x80610000 as *mut u8, 0x80600000 as *mut CloneHeader),
        }
    }
}

impl InjectState {
    pub fn new() -> Self {
        Self::default()
    }

    /// render pushes the next frame to
    /// the framebuffer
    /// it should not do anything else!
    pub fn render(&mut self) {
        self.menu.borrow_mut().draw(&mut self.render_ctxt);
        self.timer.draw(&mut self.render_ctxt);
        self.render_ctxt.draw();
    }

    /// update sets up the next frame and handles the general
    /// logic
    pub fn update(&mut self) {
        self.controller1.update();
        let menu = self.menu.clone();
        let controller1 = self.controller1.clone();
        menu.borrow_mut().update(self, &controller1);

        if !menu.borrow_mut().active() {
            menu.borrow_mut().toggle(self);
        }
    }
}
