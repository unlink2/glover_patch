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
use self::memory::*;
use core::panic::PanicInfo;

/**
 * Barebones sample
 */
#[no_mangle]
#[naked]
#[link_section = ".boot"]
pub unsafe extern "C" fn _start() -> ! {
    asm!(r#"
        b main
        nop
    "#,
    options(noreturn));
}

static mut GLOBALSTATE: Option<update::InjectState> = None;

#[no_mangle]
pub unsafe extern "C" fn main() {
    // check if game is running
    if *CURRENT_MAP == 0xFF {
        return ();
    }

	match &mut GLOBALSTATE {
		Some(state) => state.update(),
		None => GLOBALSTATE = Some(update::InjectState::new())
	}
}


/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
