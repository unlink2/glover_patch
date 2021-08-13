use super::actor::*;
use core::ffi::c_void;

pub const GAME_MODE: *mut u8 = 0x801E7530 as *mut u8;
pub const DISABLE_INPUT_TIMER: *mut u16 = 0x8028FBF4 as *mut u16;
pub const DISABLE_PAUSE_FLAG: *mut u8 = 0x801EC748 as *mut u8; // 00 disables pause
pub const PAUSE_FLAG: *mut u8 = 0x801EC7D3 as *mut u8; // 01 = paused

pub const CURRENT_MAP: *mut u8 = 0x801E7531 as *mut u8;
pub const IS_PAUSED: *mut u8 = 0x801EC7D2 as *mut u8;

pub const GLOVER_ACTOR: *mut Actor = 0x802902D8 as *mut Actor;
pub const BALL_ACTOR: *mut Actor = 0x8029F978 as *mut Actor;

pub const HEALTH: *mut i32 = 0x80290194 as *mut i32;
pub const LIVES: *mut i32 = 0x80290190 as *mut i32;

// puts fn ptr
pub type PutsXy = fn(u32, *const [char], u32);
pub const PUTS_XY: usize = 0x8014666C;
pub const PUTS_X_POS: *mut i32 = 0x801ED394 as *mut i32;
pub const PUTS_Y_POS: *mut i32 = 0x801ED398 as *mut i32;

pub const DISABLE_INTERRUPT: *const c_void = 0x801C6F70 as *const c_void;
pub const ENABLE_INTERRUPT: *const c_void = 0x801C6F90 as *const c_void;


// text is is stored in a linked list, to inject our own text we simply
// add on to the existing list
// the head object is always at this address
pub const TEXT_LL_HEAD: *const c_void = 0x80284800 as *const c_void;
pub const INSERT_TEXT_LL: *const c_void = 0x80145B98 as *const c_void;
