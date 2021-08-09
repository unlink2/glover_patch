extern crate ultrars;
use self::ultrars::input::*;
use self::ultrars::rdp::*;
use self::ultrars::font::*;
use self::ultrars::menu::*;
use super::update::*;
use super::memory::*;
use super::ultrars::memory::SharedPtrCell;

pub fn level_select_action(_entry: &mut Entry<SharedPtrCell<Trigger>>, mut trigger: SharedPtrCell<Trigger>) -> Option<usize> {
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


