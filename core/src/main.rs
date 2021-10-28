#![no_std]
#![no_main]
#![feature(lang_items)]
#![feature(start)]
#![feature(asm)]
#![feature(global_asm)]
#![feature(naked_functions)]
#![feature(alloc_error_handler)]

use linked_list_allocator::LockedHeap;

#[global_allocator]
static ALLOCATOR: LockedHeap = LockedHeap::empty();

extern crate alloc;

pub mod actions;
pub mod actor;
pub mod camera;
pub mod cheatsmenu;
pub mod gdbmenu;
pub mod mainmenu;
pub mod memory;
pub mod renderer;
pub mod update;
use self::memory::*;
use core::panic::PanicInfo;

static mut GLOBALSTATE: Option<update::InjectState> = None;
static mut ZEROED_RAM: bool = false;

#[no_mangle]
#[link_section = ".boot"]
pub unsafe extern "C" fn _start(arg: usize) -> () {
    // check if game is running
    if *CURRENT_MAP == 0xFF {
        // zero static ram space
        if !ZEROED_RAM {
            ZEROED_RAM = true;
            ultrars::memory::umemset(0x80500000 as *mut u8, 0, 0x200000);
            // init heap
            init_heap(0x80500000, 0x200000);
        }
        return;
    }

    match &mut GLOBALSTATE {
        Some(state) => {
            if arg == 1 {
                state.render();
            } else {
                state.update();
            }
        }
        None => GLOBALSTATE = Some(update::InjectState::new()),
    }

    return;
}

/// This function must be called before the heap is usable!
pub fn init_heap(address: usize, size: usize) {
    unsafe {
        ALLOCATOR.lock().init(address, size);
    }
}

/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[alloc_error_handler]
fn alloc_error(_: core::alloc::Layout) -> ! {
    panic!()
}
