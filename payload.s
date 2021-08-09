.n64
.create "./target/mips-bin/payload.bin", 0xB0780000

.org 0xB0780000

// jump here to get to c code
// TODO dma to ram to have string constants
C_CODE_START equ 0x80400200 //(0xB0780200)

// function that was replaced by inject
ORIGINAL equ 0x80178E98

DMA_RAM equ 0x80400000
DMA_ROM equ 0xB0780000
DMA_SIZE equ 0xFFFF

RENDER_RA equ 0x8017FF1C

// mode is based on ra
// inputs:
//  ra == 8013E854 -> call render code
section_code:
    la t0, RENDER_RA
    bne ra, t0, not_render_mode
    nop
    b render_inject
    nop
not_render_mode:


	// call the original function we replaced
	la t3, ORIGINAL
	jalr t3 // far call
	nop


    // jump to c code in logic mode
    la a0, 0x00
    la ra, C_CODE_START
    jalr ra
    nop


	// return
    la ra, 0x8013F378 // return address
	jr ra // return
    lui v0, 0x801F // nop // need a nop after jr


// inject location during rendering
// must be pretty fast, only do actual rendering here
render_inject:
    // jump to c code in rendering mode
    la a0, 0x01
    la ra, C_CODE_START
    jalr ra
    nop

    // call swap buffer function (new inject)
    lw  v0, 0x0014(s8)
    lw  a0, 0x000C(v0)
    la ra, 0x801CF070
    jalr ra
    nop

    // jump to cde in update mode
    // la a0, 0x00
    // la ra, C_CODE_START
    // jalr ra
    // nop

    // load return address
    la ra, RENDER_RA
    jr ra
    nop


// section_data:
// text:
//  .string "Glove is love!", 0x00

.close
