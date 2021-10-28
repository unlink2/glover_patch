use super::update::Trigger;
use ultrars::memory::SharedPtrCell;
use ultrars::menu::Entry;
use ultrars::usb::*;

pub fn enable_gdb_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    trigger.as_mut().usb.init();
    entry.set_checkbox(true);
    return None;
}
