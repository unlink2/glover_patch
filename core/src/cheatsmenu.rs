use super::update::Trigger;
use crate::memory::{
    self, COLLISION_DISABLE, DEBUG_GRAPH, DEMO_END_TIMER, DISABLE_PAUSE_FLAG, FOG,
    INFINITE_DOUBLE_JUMP,
};
use crate::ultrars::memory::SharedPtrCell;
use crate::ultrars::menu::Entry;

// TODO modify live/hp code instead of using flags
pub fn lives_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    let prev = trigger.as_ref().inf_lives;
    trigger.as_mut().inf_lives = !prev;

    entry.set_checkbox(!prev);

    return None;
}

pub fn hp_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    let prev = trigger.as_ref().inf_hp;
    trigger.as_mut().inf_hp = !prev;

    entry.set_checkbox(!prev);

    return None;
}

pub fn fog_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        *FOG = !*FOG;
        entry.set_checkbox(*FOG == 1);
    }
    return None;
}

pub fn collision_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        if *COLLISION_DISABLE == 0x00000000 {
            // restore code
            *COLLISION_DISABLE = 0x0C057828;
        } else {
            *COLLISION_DISABLE = 0x000000000; // nop
        }
        entry.set_checkbox(*COLLISION_DISABLE == 0x00000000);
    }
    return None;
}

pub fn demo_end_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        *DEMO_END_TIMER = 1;
        entry.set_checkbox(*DEMO_END_TIMER != 0);
    }
    return None;
}

pub fn debug_graph_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        *DEBUG_GRAPH = !*DEBUG_GRAPH;
        entry.set_checkbox(*DEBUG_GRAPH == 1);
    }
    return None;
}

pub fn disable_pause_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        *DISABLE_PAUSE_FLAG = !*DISABLE_PAUSE_FLAG;
        entry.set_checkbox(*DISABLE_PAUSE_FLAG == 0);
    }
    return None;
}

pub fn inf_dobule_jump_cheat_action(
    entry: &mut Entry<SharedPtrCell<Trigger>>,
    mut trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    trigger.as_mut().infite_jump = !trigger.as_ref().infite_jump;
    entry.set_checkbox(trigger.as_ref().infite_jump);
    return None;
}

pub fn summon_ball_action(
    _entry: &mut Entry<SharedPtrCell<Trigger>>,
    _trigger: SharedPtrCell<Trigger>,
) -> Option<usize> {
    unsafe {
        (*memory::BALL_ACTOR).position = (*memory::GLOVER_ACTOR).position;
    }
    return None;
}
