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
use core::panic::PanicInfo;
/**
 * Barebones sample
 */
#[no_mangle]
#[naked]
pub unsafe extern "C" fn _start() -> ! {
    asm!(r#"
        b main
    "#,
    options(noreturn));
}

static mut GLOBALSTATE: Option<update::InjectState> = None;

#[no_mangle]
pub unsafe extern "C" fn main() -> () {
	match &mut GLOBALSTATE {
		Some(state) => state.update(),
		None => GLOBALSTATE = Some(update::InjectState::new())
	}
    return ();
}


/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
