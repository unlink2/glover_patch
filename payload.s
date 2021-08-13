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
	la t3, 0x8014D3AC
	jalr t3 // far call
	nop


    // jump to code in logic mode
    la a0, 0x00
    la ra, C_CODE_START
    jalr ra
    nop
    // jump to code in rendering mode
    la a0, 0x01
    la ra, C_CODE_START
    jalr ra
    nop

	// return
    la ra, 0x8013E85C + 12 // return address
	jr ra // return
    addu a0, r0, r0

// section_data:
// text:
//  .string "Glove is love!", 0x00

.close
