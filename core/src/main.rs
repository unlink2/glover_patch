#![no_std]
#![no_main]
#![feature(lang_items)]
#![feature(start)]
#![feature(asm)]
#![feature(global_asm)]
#![feature(naked_functions)]

extern crate ultrars;
pub mod update;
pub mod memory;
pub mod mainmenu;
pub mod actor;
pub mod camera;
pub mod renderer;
use self::memory::*;
use core::panic::PanicInfo;


static mut GLOBALSTATE: Option<update::InjectState> = None;

#[no_mangle]
#[link_section = ".boot"]
pub unsafe extern "C" fn _start(arg: usize) -> () {
    // check if game is running
    if *CURRENT_MAP == 0xFF {
        return;
    }

	match &mut GLOBALSTATE {
		Some(state) => if arg == 1 { state.render(); } else { state.update(); },
		None => GLOBALSTATE = Some(update::InjectState::new())
	}

    return;
}

/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
