#![no_std]
#![feature(naked_functions)]
#![feature(asm_experimental_arch)]
#![feature(asm_const)]
extern crate alloc;

pub mod clone;
pub mod color;
pub mod font;
pub mod frameadvance;
pub mod init;
pub mod input;
pub mod interrupt;
pub mod keyboard;
pub mod malloc;
pub mod math;
pub mod memory;
pub mod menu;
pub mod monitor;
pub mod rdp;
pub mod render;
pub mod timer;
pub mod usb;
pub mod watch;
