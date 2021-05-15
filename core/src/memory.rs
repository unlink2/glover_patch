use super::actor::*;
use super::camera::*;

pub const GAME_MODE: *mut u8 = 0x801E7530 as *mut u8;
pub const DISABLE_INPUT_TIMER: *mut u16 = 0x8028FBF4 as *mut u16;
pub const DISABLE_PAUSE_FLAG: *mut u8 = 0x801EC748 as *mut u8;

pub const CURRENT_MAP: *mut u8 = 0x801E7531 as *mut u8;
pub const IS_PAUSED: *mut u8 = 0x801EC7D2 as *mut u8;

pub const GLOVER_ACTOR: *mut Actor = 0x802902D8 as *mut Actor;
pub const BALL_ACTOR: *mut Actor = 0x8029F978 as *mut Actor;

pub const HEALTH: *mut i32 = 0x802902D8 as *mut i32;
pub const LIVES: *mut i32 = 0x8029F978 as *mut i32;
