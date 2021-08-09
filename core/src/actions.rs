use crate::ultrars::menu::{Menu, Entry};
use super::update::{Trigger, MenuType};
use crate::ultrars::memory::SharedPtrCell;
use super::memory::*;

pub fn open_menu(_entry: &mut Entry<SharedPtrCell<Trigger>>, _trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 1;
    }
    return None;
}

pub fn close_menu(_entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 0;
        trigger.as_mut().toggle = true;
    }
    return None;
}

pub fn update_menu(_entry: &mut Entry<SharedPtrCell<Trigger>>, _trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    unsafe {
        *IS_PAUSED = 1;
    }
    return None;
}

pub fn cheats_action(_entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    let trigger = trigger.as_mut();
    trigger.menu_type = MenuType::CheatMenu;
    trigger.load_menu = true;
    return None;
}

pub fn main_action(_entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    let trigger = trigger.as_mut();
    trigger.menu_type = MenuType::MainMenu;
    trigger.load_menu = true;
    return None;
}

// TODO modify live/hp code instead of using flags
pub fn lives_cheat_action(entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    let prev = trigger.as_ref().inf_lives;
    trigger.as_mut().inf_lives = !prev;

    entry.set_checkbox(!prev);

    return None;
}

pub fn hp_cheat_action(entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
    let prev = trigger.as_ref().inf_hp;
    trigger.as_mut().inf_hp = !prev;

    entry.set_checkbox(!prev);

    return None;
}

