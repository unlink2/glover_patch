use crate::ultrars::menu::Entry;
use super::update::Trigger;
use crate::ultrars::memory::SharedPtrCell;


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

