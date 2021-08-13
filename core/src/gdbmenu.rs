use super::update::Trigger;
use crate::ultrars::memory::SharedPtrCell;
use crate::ultrars::menu::Entry;
use crate::ultrars::usb::*;

pub fn enable_gdb_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    trigger.as_mut().usb.init();
    entry.set_checkbox(true);
    return None;
}
