use crate::update::InjectState;

use super::memory::*;
use super::update::Trigger;
use ultrars::memory::SharedPtrCell;
use ultrars::menu::Entry;

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

pub fn open_menu(_trigger: &mut InjectState) -> Option<usize> {
    pause();
    return None;
}

pub fn close_menu(state: &mut InjectState) -> Option<usize> {
    unpause();
    return None;
}

pub fn update_menu(_trigger: InjectState) -> Option<usize> {
    pause();
    return None;
}
