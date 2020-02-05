arch n64.cpu
endian msb
output "./bin/payload.bin", create

origin $00000000
base $B0780000

include "lib/N64.INC" // Include N64 Definitions
// include "lib/N64_GFX.INC" // gfx macros

// jump here to get to c code
// TODO dma to ram to have string constants
constant C_CODE_START($B0790000)

constant INPUTS_HI($BFC0)
constant INPUTS_LO($07C4)
constant INPUTS_LOP2($07CC)

constant L_INPUT($15)
constant CL_INPUT($11)
constant CR_INPUT($10)
constant CD_INPUT($12)
constant CU_INPUT($13)
constant A_INPUT($FF)
constant B_INPUT($1E)
constant START_INPUT($1C)

constant INJECTED_RAM_HI($8000)
constant EXP_RAM_HI($8040)
constant X_BAC($02B0)
constant Y_BAC($02B4)
constant Z_BAC($02B8)
constant CAMX_BAC($02BC)
constant CAMY_BAC($02C0)
constant CAMZ_BAC($02C4)
constant pframe_last($02C8) // last frames inputs, player2
constant pframe_advance($02CC) // if 01 = enable frame advance
constant STRING_BUFFER($800002E0) // 32 bytes for strings to be printed
// constant GP_BUFFER($80500000)

constant GLOVER_XYZ_HI($8029)
constant GLOVER_X_LO($030C)
constant GLOVER_Y_LO($0310)
constant GLOVER_Z_LO($0314)
constant GLOVER_VEL_X($0354)
constant GLOVER_VEL_Y($0338)
constant GLOVER_VEL_Z($033C)

constant CAM_XYZ_HI($8020)
constant CAM_X_LO($2254)
constant CAM_Y_LO($2258)
constant CAM_Z_LO($225C)

constant CAMR_XYZ_HI($8029)
constant CAMR_X_LO($F920)
constant CAMR_Y_LO($F924)
constant CAMR_Z_LO($F928)

constant BALL_XYZ_HI($802A)
constant BALL_X_LO($F9AC)
constant BALL_Y_LO($F9B0)
constant BALL_Z_LO($F9B4)
constant BALL_VEL_X($FA24)
constant BALL_VEL_Y($FA2C)
constant BALL_VEL_Z($FA30)
constant BALL_ROLL_VEL_X($F9D0)
constant BALL_ROLL_VEL_Y($F9D8)
constant BALL_ROLL_VEL_Z($F9DC)

constant FILE1_HI($801E0000)
constant FILE1_START($801EAA44)
constant FILE1_LEVELS($801EAA48)

constant TIMER_HW($801FA754)
constant RNG($801ED3F0)
constant MAP_ID($801E7531) // byte

constant GAME_MODE($801E7530) // byte 04 = gameplay 02 = level select
constant GAME_RAW_INPUTS($801FB280)
constant INPUT_HANDLER($8013407C)
constant LAST_Z_WRITE($801349B4)

constant ACTOR_HEAP_START($802902D8)
constant ACTOR_SIZE($F0) // bytes to copy per actor
// evaluate ACTOR_HEAP_SIZE((ACTOR_HEAP_END-ACTOR_HEAP_START+$04)/$04)

constant ACTOR_HEAP_CLONE($80400000)

constant STRCPY($801468EC)
constant ORIGINAL($80178E98)

// hi bytes of data address
evaluate CODE_MASK(section_code >> 16)
evaluate DATA_MASK(section_data >> 16)

// reads inputs, shifts by N shifts
// then ands
// side effects:
//	uses t3 and t1
// returns
// 	t1 == 0 if not pressed
//	t1 != 0 if pressed
macro read_input_n(shift, register) {
	lui t3, INPUTS_HI // inputs bits
	lw t1, {register}(t3) // load inputs value
	srl t1, t1, {shift} // shift by N bits to get the pressed bit in the right position
	andi t1, t1, $01  // check button press
}

macro read_input(shift) {
    read_input_n({shift}, INPUTS_LO)
}

// reads last frames stored inputs
// returns:
//  t1 == 0 if not pressed last frame
//  t1 != 0 if pressed
macro last_input_n(shift, register) {
	lui t3, INJECTED_RAM_HI // inputs bits
	lw t1, {register}(t3) // load inputs value
	srl t1, t1, {shift} // shift by N bits to get the pressed bit in the right position
	andi t1, t1, $01  // check button press
}


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

    // ported to C
    // jal frame_advance
    // nop

	// check start input
	read_input(START_INPUT)
	blez t1, not_start
	nop

	read_input(CL_INPUT)
	blez t1, not_CL
	nop // delay slot

	lui t3, GLOVER_XYZ_HI // address for glover ram start
	lui t1, INJECTED_RAM_HI

	lw t2, GLOVER_X_LO(t3) // load X position
	sw t2, X_BAC(t1)

	lw t2, GLOVER_Y_LO(t3) // load Y position
	sw t2, Y_BAC(t1)

	lw t2, GLOVER_Z_LO(t3) // load Z position
	sw t2, Z_BAC(t1)

	// camera x/y/z
	lui t3, CAM_XYZ_HI
	lw t2, CAM_X_LO(t3)
	sw t2, CAMX_BAC(t1)

	lw t2, CAM_Y_LO(t3)
	sw t2, CAMY_BAC(t1)

	lw t2, CAM_Z_LO(t3)
	sw t2, CAMZ_BAC(t1)

not_CL:

	read_input(CR_INPUT)
	blez t1, not_CR
	nop // delay slot

	lui t3, GLOVER_XYZ_HI // address for glover ram start
	lui t1, INJECTED_RAM_HI
	lui t4, BALL_XYZ_HI

	lw t2, X_BAC(t1) // load X position
	sw t2, GLOVER_X_LO(t3)
	sw t2, BALL_X_LO(t4)

	lw t2, Y_BAC(t1) // load Y position
	sw t2, GLOVER_Y_LO(t3)
	sw t2, BALL_Y_LO(t4)

	lw t2, Z_BAC(t1) // load Z position
	sw t2, GLOVER_Z_LO(t3)
	sw t2, BALL_Z_LO(t4)

	// stop glover
	lui t2, $00
	sw t2, GLOVER_VEL_X(t3)
	sw t2, GLOVER_VEL_Y(t3)
	sw t2, GLOVER_VEL_Z(t3)
	sw t2, BALL_VEL_X(t4)
	sw t2, BALL_VEL_Y(t4)
	sw t2, BALL_VEL_Z(t4)
	sw t2, BALL_ROLL_VEL_X(t4)
	sw t2, BALL_ROLL_VEL_Y(t4)
	sw t2, BALL_ROLL_VEL_Z(t4)

	// camera x/y/z
	lui t3, CAM_XYZ_HI
	lw t2, CAMX_BAC(t1)
	sw t2, CAM_X_LO(t3)

	lw t2, CAMY_BAC(t1)
	sw t2, CAM_Y_LO(t3)

	lw t2, CAMZ_BAC(t1)
	sw t2, CAM_Z_LO(t3)

	// camera x/y/z
	lui t3, CAMR_XYZ_HI
	lw t2, CAMX_BAC(t1)
	sw t2, CAMR_X_LO(t3)

	lw t2, CAMY_BAC(t1)
	sw t2, CAMR_Y_LO(t3)

	lw t2, CAMZ_BAC(t1)
	sw t2, CAMR_Z_LO(t3)
not_CR:
	read_input(CU_INPUT)
	blez t1, not_CU
	nop

	jal copy_actors
	nop
not_CU:

	read_input(CD_INPUT)
	blez t1, not_CD
	nop

	jal restore_actors
	nop
not_CD:
	read_input(A_INPUT)
	blez t1, not_A
	nop
    // ported to C code
	// jal enable_timer
	nop
not_A:
    read_input(B_INPUT)
    blez t1, not_B
    nop
    // ported to C code
    // jal level_select
    nop
not_B:
not_start:
	// init check
	jal clear_file1
	nop

	// return
	lui t3, {DATA_MASK} // set up direct load for return address
	lw ra, return(t3) // load it
	jr ra // return
	nop // need a nop after jr
}

// finds all actors
// in the heap starting at glover
// all actors loop on each other and are stored in a linked list
// and memcpys them using the following format:
// actor backup format:
// 	4 bytes for original start address
//	4 bytes of actor size
// 	ACTOR_SIZE of data
//	List ends with a word of $00
//	at the end of the list the following helper values
//	are stored:
//		RNG - WORD
// uses A1 as the pointer to the next free backup heap location
// each actor is ACTOR_SIZE bytes
// the backup heap is locates at the start of exp pack memory
copy_actors:
scope {
	// set up all values we need
	// avoid t1, t0, a0, a1 and a2
	// because memcpy uses those
	la t4, ACTOR_HEAP_START
	la t6, ACTOR_HEAP_START // end value for bne
	la t5, $01 // -1
	la t7, $04 // word size
	la a1, ACTOR_HEAP_CLONE

	// search heap by the word
copy_actor_loop:
	// next address to copy
	move a0, t4

	lw t4, $00(t4) // next ptr

	// store src ptr
	sw a0, $00(a1)
	addu a1, a1, t7 // next dest ptr

	la a2, ACTOR_SIZE
	sw a2, $00(a1)
	addu a1, a1, t7 // next dest ptr

	// memcpy increments a1 so we do not have to do it here
	// hold return address in v1 for now
	move v1, ra
	jal memcpy
	nop
	move ra, v1
	// the actor list loops on itself so when we have reached the start
	// actor we are done!
	bne t4, t6, copy_actor_loop
	nop
copy_actor_done:

	// end list with 1 word of $00
	sw r0, $00(a1)
	// save rng value
	la t0, RNG
	lw t1, $00(t0)
	sw t1, $04(a1)

	jr ra
	nop
}

// reverses actor backup
// uses A0 as actor heap pointer
restore_actors:
scope {
	// start of clone list
	la a0, ACTOR_HEAP_CLONE
	la t5, $04 // +1 word
	la t6, ACTOR_SIZE // +110

restore_actor_loop:
	// load address
	lw a1, $00(a0)
	// if a1 is zero end of list
	beq a1, r0, restore_actor_done
	nop

	addu a0, a0, t5 // add 1 to src
	move t7, ra
	lw a2, $00(a0)
	addu a0, a0, t5 // next word

	jal memcpy
	nop
	move ra, t7
	j restore_actor_loop
	nop
restore_actor_done:
	// get rng value
	la t0, RNG
	lw t1, $04(a0) // RNG value
	sw t1, $00(t0)

	jr ra
	nop
}

// copies a block of memory from 
// one address to another 
// inputs:
// 	A0 -> src
//	A1 -> dest 
//	A2 -> size
memcpy:
scope {
	la t1, $01
loop:
	lb t0, $00(a0)
	sb t0, $00(a1)
	addu a0, a0, t1
	addu a1, a1, t1
	sub a2, a2, t1
	bne a2, r0, loop
	nop // delay
	jr ra
	nop // delay
}

// clears file 1
clear_file1:
scope {
	la t0, FILE1_LEVELS
	lw t0, $00(t0)
	la t1, $FFFFFFFF
	beq t0, t1, file1_init_done
	nop

	// init file with memcpy
	la a1, FILE1_START
	la t2, $04
	la t1, $4B4C4D00
	sw t1, $00(a1)

	addu a1, a1, t2
	la t1, $FFFFFFFF
	sw t1, $00(a1)
	addu a1, a1, t2
	sw t1, $00(a1)

	addu a1, a1, t2
	la t1, $00000190
	sw t1, $00(a1)

	addu a1, a1, t2
	la t1, $0A000606
	sw t1, $00(a1)

	addu a1, a1, t2
	la t1, $40000001
	sw t1, $00(a1)

	addu a1, a1, t2
	la t1, $006E006E
	sw t1, $00(a1)
file1_init_done:
	jr ra
	nop

}

// enables a timer
enable_timer:
scope {
	la t1, $FF
	la t3, TIMER_HW
	sb r0, $00(t3)
	sb t1, $01(t3)

	jr ra
	nop
}

// calls level select
level_select:
scope {
    addi t0, r0, $02
    la t1, GAME_MODE
    sb t0, $00(t1)

    la t1, $801EC7D3
    sb r0, $00(t1) // stop pause state

    jr ra
    nop
}

// frame advance delay loop
frame_advance:
scope {
    lui t6, INJECTED_RAM_HI
    lb t7, pframe_advance(t6)
    la t1, $01


    // toggle if c up on p2
    read_input_n(CU_INPUT, INPUTS_LOP2)
    beq r0, t1, no_swap
    nop

    // check previous input
    move t2, t1
    last_input_n(CU_INPUT, pframe_last)
    xor t1, t2
    beq r0, t1, no_swap

    // toggle
    lb t4, pframe_advance(t6)
    la t2, $01
    xor t1, t4, t2
    sb t1, pframe_advance(t6)
    b frame_advance_done

no_swap:

    // advance if down on p2
    read_input_n(CD_INPUT, INPUTS_LOP2)
    beq r0, t1, no_advance

    // check previous input
    move t2, t1
    last_input_n(CD_INPUT, pframe_last)
    xor t1, t2
    bne r0, t1, frame_advance_done
    nop
no_advance:
    // store last inputs during the loop as well
    lui t2, INPUTS_HI
    lw t2, INPUTS_LOP2(t2)
    sw t2, pframe_last(t6)

skip_inputs:
    beq r0, t7, frame_advance_done
    nop

    // replace with nop
    la t5, LAST_Z_WRITE
    sw r0, $00(t5) // replace with nop
    b frame_advance
    nop

frame_advance_done:

    // restore input handler code
    la t5, LAST_Z_WRITE
    la t4, $A0440000 // original instruction
    sw t4, $00(t5)

    // propably not needed
    // just store last inputs again just in case
    lui t2, INPUTS_HI
    lw t2, INPUTS_LOP2(t2)
    sw t2, pframe_last(t6)

    jr ra
    nop
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
file_1:
  dw $4B4C4D00
  dw $FFFFFFFF
  dw $FFFFFFFF
  dw $00000190
  dw $0A000606
  dw $40000001
  dw $006E006E
align($04)
return:
  dw $8013F378

align(8) // Align 64-Bit
rdp_buffer:
arch n64.rdp
