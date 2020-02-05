arch n64.cpu
endian msb
output "./bin/payload.bin", create

origin $00000000
base $B0780000

include "lib/N64.INC" // Include N64 Definitions

// jump here to get to c code
// TODO dma to ram to have string constants
constant C_CODE_START($B0790000)

constant STRING_BUFFER($800002E0) // 32 bytes for strings to be printed

// function that was replaced by inject
constant ORIGINAL($80178E98)


// call string print
macro far_call_print(str_ptr, x, y) {
	// call print function
    la a1, {str_ptr}
	la a0, $80202240 // value from other calls
	la a2, $801ED324 // value from other calls
	la t3, $8014666C // function ptr

    la t0, $801ED394 // x coordinate
    la t1, {x}
    sw t1, $00(t0) // store x
    la t1, {y}
    sw t1, $04(t0) // store y

	jalr t3 // far call
	nop // need a nop after jr
}

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
    far_call_print(STRING_BUFFER, $55, $55)

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

