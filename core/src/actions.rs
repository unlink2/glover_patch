use crate::ultrars::menu::Entry;
use super::update::Trigger;
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

