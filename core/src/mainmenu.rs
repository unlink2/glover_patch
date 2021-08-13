use super::memory::*;
use super::ultrars::memory::SharedPtrCell;
use super::ultrars::menu::*;
use super::update::*;

pub fn level_select_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        *DISABLE_PAUSE_FLAG = 0x00;
        *DISABLE_INPUT_TIMER = 0x00;
        *GAME_MODE = 0x02;
        *PAUSE_FLAG = 0x00;
    }
    let t = trigger.as_mut();
    t.toggle = true;
    return None;
}

pub fn cheats_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    let trigger = trigger.as_mut();
    trigger.menu_type = MenuType::CheatMenu;
    trigger.load_menu = true;
    return None;
}

pub fn gdb_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    let trigger = trigger.as_mut();
    trigger.menu_type = MenuType::GdbMenu;
    trigger.load_menu = true;
    return None;
}

pub fn main_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    let trigger = trigger.as_mut();
    trigger.menu_type = MenuType::MainMenu;
    trigger.load_menu = true;
    return None;
}

pub fn monitor_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    return None;
}
