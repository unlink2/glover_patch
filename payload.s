.n64
.create "./target/mips-bin/payload.bin", 0x80400000

.org 0x80400000

// jump here to get to c code
// TODO dma to ram to have string constants
C_CODE_START equ 0x80400200 //(0xB0780200)

// function that was replaced by inject
ORIGINAL equ 0x80178E98

DMA_RAM equ 0x80400000
DMA_ROM equ 0xB0780000
DMA_SIZE equ 0xFFFF

RENDER_RA equ 0x8017FF1C

.macro store_reg, regnum
    subi sp, sp, 4
    sw $regnum, (sp)
.endmacro

.macro load_reg, regnum
    lw $regnum, (sp)
    addiu sp, sp, 4
.endmacro

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

    // bal save_registers
    // nop

    // jump to c code in rendering mode
    la a0, 0x01
    la ra, C_CODE_START
    jalr ra
    nop

    // jump to c code in logic mode
    la a0, 0x00
    la ra, C_CODE_START
    jalr ra
    nop

    // bal load_registers
    // nop

	// return
    la ra, 0x8013F378 // return address
	jr ra // return
    lui v0, 0x801F // nop // need a nop after jr


// inject location during rendering
// must be pretty fast, only do actual rendering here
render_inject:
    // call swap buffer function (new inject)
    lw  v0, 0x0014(s8)
    lw  a0, 0x000C(v0)
    la ra, 0x801CF070
    jalr ra
    nop

    // bal save_registers
    // nop

    // jump to c code in rendering mode
    la a0, 0x01
    la ra, C_CODE_START
    jalr ra
    nop

    // jump to cde in update mode
    la a0, 0x00
    la ra, C_CODE_START
    jalr ra
    nop

    // bal load_registers
    // nop

    // load return address
    la ra, RENDER_RA
    jr ra
    nop

save_registers:
    // $v
    store_reg 2
    store_reg 3

    // $a
    store_reg 4
    store_reg 5
    store_reg 6
    store_reg 7

    // s0-s7
    store_reg 16
    store_reg 17
    store_reg 18
    store_reg 19
    store_reg 20
    store_reg 21
    store_reg 22
    store_reg 23

    // gp
    store_reg 28

    // fp
    store_reg 30

    jr ra
    nop

load_registers:
    // fp
    load_reg 30

    // gp
    load_reg 28

    // s0-s7
    load_reg 23
    load_reg 22
    load_reg 21
    load_reg 20
    load_reg 19
    load_reg 18
    load_reg 17
    load_reg 16

    // $a
    load_reg 7
    load_reg 6
    load_reg 5
    load_reg 4

    // $v
    load_reg 3
    load_reg 2

    jr ra
    nop

// section_data:
// text:
//  .string "Glove is love!", 0x00

.close
