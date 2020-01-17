arch n64.cpu
endian msb
output "./bin/payload.bin", create

origin $00000000
base $B0780000

include "lib/N64.INC" // Include N64 Definitions

constant INPUTS_HI($BFC0)
constant INPUTS_LO($07C4)

constant L_INPUT($15)
constant CL_INPUT($11)
constant CR_INPUT($10)
constant CD_INPUT($12)
constant CU_INPUT($13)
constant A_INPUT($FF)
constant START_INPUT($1C)

constant INJECTED_RAM_HI($8000)
constant EXP_RAM_HI($8040)
constant X_BAC($02B0)
constant Y_BAC($02B4)
constant Z_BAC($02B8)
constant CAMX_BAC($02BC)
constant CAMY_BAC($02C0)
constant CAMZ_BAC($02C4)

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
macro read_input(shift) {
	lui t3, INPUTS_HI // inputs bits 
	lw t1, INPUTS_LO(t3) // load inputs value
	srl t1, t1, {shift} // shift by N bits to get the pressed bit in the right position 
	andi t1, t1, $01  // check button press
}

// removes the pause menu as soon as it wants to appear 
macro disable_pause() {

}

// calls strcpy
// copies a string from a1 to a0
macro far_call_strcpy(from, to) {
	// call print function
	la a0, {to} 
	la a1, {from} 
	la t3, STRCPY 
	jalr t3 // far call
	nop // need a nop after jr
}

section_code:
	// call the original function we replaced
	la t3, ORIGINAL
	jalr t3 // far call
	nop


	// far_call_strcpy($8010271C, $802B2258)

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
	jal enable_timer
	nop
not_A:
not_start:
	// init check 
	jal clear_file1
	nop

	// return
	lui t3, {DATA_MASK} // set up direct load for return address
	lw ra, return(t3) // load it 
	jr ra // return
	nop // need a nop after jr

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

// reverses actor backup
// uses A0 as actor heap pointer
restore_actors:
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

// copies a block of memory from 
// one address to another 
// inputs:
// 	A0 -> src
//	A1 -> dest 
//	A2 -> size
memcpy:
	la t1, $01
memcpy_loop:
	lb t0, $00(a0)
	sb t0, $00(a1)
	addu a0, a0, t1
	addu a1, a1, t1
	sub a2, a2, t1
	bne a2, r0, memcpy_loop
	nop // delay 
	jr ra
	nop // delay 

// clears file 1
clear_file1:
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

// enables a timer 
enable_timer:
	la t1, $FF
	la t3, TIMER_HW 
	sb r0, $00(t3)
	sb t1, $01(t3)
	jr ra
	nop

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

