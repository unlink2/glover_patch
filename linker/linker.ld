/**
This file is part of https://github.com/rust-console/cargo-n64

Copyright 2018 Jay Oster

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

ENTRY(_start)

SECTIONS {
    . = 0x80400200;
    __boot_start = .;

    .boot : {
        *(.boot)
        . = ALIGN(16);
    }

    .text : {
        *(.text .text.*)
        . = ALIGN(16);
    }
	
    .rodata : {
        *(.rodata .rodata.*)
        . = ALIGN(16);
    }

    .data : {
        *(.data .data.*)
        . = ALIGN(16);
    }

    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        . = ALIGN(16);
        __bss_end = .;
    }
   
   .eh_frame (INFO) : { KEEP(*(.eh_frame)) }
   .eh_frame_hdr (INFO) : { *(.eh_frame_hdr) }

    __rom_end = . - __boot_start + 0xB0001000;

    /DISCARD/ : {
        *(.MIPS.*)
        *(.comment)
        *(.mdebug.*)
        *(.pdr)
        *(.reginfo)

        /*
         * We may need the global offset table some day.
         * Our target is currently set with a static relocation-model, so this
         * might not be needed after all.
         */
        *(.got)
    }
}
