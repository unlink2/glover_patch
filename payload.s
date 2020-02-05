arch n64.cpu
endian msb
output "./bin/payload.bin", create

origin $00000000
base $B0780000

include "lib/N64.INC" // Include N64 Definitions

// jump here to get to c code
// TODO dma to ram to have string constants
constant C_CODE_START($B0790000)

// function that was replaced by inject
constant ORIGINAL($80178E98)



// mode is based on ra
// inputs:
//  ra == 8013E854 -> call render code
section_code:
scope {
    la t0, $8013E850
    bne ra, t0, not_render_mode
    nop
    j render_inject
    nop
not_render_mode:

	// call the original function we replaced
	la t3, ORIGINAL
	jalr t3 // far call
	nop


    // jump to c code in logic mode
    la a0, $00
    la ra, C_CODE_START
    jalr ra
    nop


	// return
    la ra, $8013F378 // return address
	jr ra // return
	nop // need a nop after jr
}


// inject location during rendering
// must be pretty fast, only do actual rendering here
render_inject:
scope {
    // call original functions
    la t0, $8014AD0C
    jalr t0
    nop
    la t0, $80145EAC
    jalr t0
    nop

    // jump to c code in rendering mode
    la a0, $01
    la ra, C_CODE_START
    jalr ra
    nop

    // load return address
    la ra, $8013E850
    jr ra
    nop

}

section_data:
text:
  db "Glove is love!", $00

