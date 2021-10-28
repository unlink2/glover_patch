use super::memory::*;
use super::update::Trigger;
use crate::ultrars::memory::SharedPtrCell;
use crate::ultrars::menu::Entry;

pub fn pause() {
    unsafe {
        *IS_PAUSED = 1;
    }
}

pub fn unpause() {
    unsafe {
        *IS_PAUSED = 0;
    }
}

pub fn freeze(mut cell: SharedPtrCell<Trigger>) {
    unsafe {
        if *GAME_MODE != 3 {
            cell.as_mut().original_game_mode = *GAME_MODE;
            *GAME_MODE = 3;
        }
    }
}

pub fn unfreeze(cell: SharedPtrCell<Trigger>) {
    unsafe {
        *GAME_MODE = cell.as_ref().original_game_mode;
    }
}

pub fn open_menu(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    pause();
    return None;
}

pub fn close_menu(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unpause();
    trigger.as_mut().toggle = true;
    return None;
}

pub fn update_menu(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    pause();
    return None;
}
