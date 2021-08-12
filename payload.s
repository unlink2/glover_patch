.n64
.create "./target/mips-bin/payload.bin", 0x80400000

.org 0x80400000

// jump here to get to c code
C_CODE_START equ 0x80400200

// mode is based on ra
// inputs:
//  ra == 8013E854 -> call render code
section_code:
	// call the original function we replaced
	la t3, 0x80178E98
	jalr t3 // far call
	nop

    // bal save_registers
    // nop


    // jump to c code in logic mode
    la a0, 0x00
    la ra, C_CODE_START
    jalr ra
    nop
    // jump to c code in rendering mode
    la a0, 0x01
    la ra, C_CODE_START
    jalr ra
    nop

    // bal load_registers
    // nop

	// return
    la ra, 0x8013F378 // return address
	jr ra // return
    lui v0, 0x801F // nop // need a nop after jr

// section_data:
// text:
//  .string "Glove is love!", 0x00

.close
