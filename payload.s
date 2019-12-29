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
constant START_INPUT($1C)

constant INJECTED_RAM_HI($8000)
constant EXP_RAM_HI($8040)
constant X_BAC($02B0)
constant Y_BAC($02B4)
constant Z_BAC($02B8)

constant GLOVER_XYZ_HI($8029)
constant GLOVER_X_LO($030C)
constant GLOVER_Y_LO($0310)
constant GLOVER_Z_LO($0314)
constant GLOVER_VEL_X($0354)
constant GLOVER_VEL_Y($0338)
constant GLOVER_VEL_Z($033C)

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


constant ACTOR_HEAP_START($8028FFFC)
constant ACTOR_HEAP_END($803FFFFF) 
// actor visible flag, search for this to find actors, search value is alwaus $20 bytes away from copy start
constant ACTOR_SEARCH_VALUE($64) 
constant ACTOR_OFFSET($3C) // offset from search value to copy start
constant ACTOR_SIZE($110) // bytes to copy per actor 
evaluate ACTOR_HEAP_SIZE((ACTOR_HEAP_END-ACTOR_HEAP_START+$04)/$04)

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
not_start:
	// return
	lui t3, {DATA_MASK} // set up direct load for return address
	lw ra, return(t3) // load it 
	jr ra // return
	nop // need a nop after jr

// finds all actors 
// in the heap
// based on a given search value ($64)
// and memcpys them using the following format:
// actor backup format:
// 	4 bytes for original start address
// 	ACTOR_SIZE of data
//	List ends with a word of $00
// uses A1 as the pointer to the next free backup heap location
// each actor is ACTOR_SIZE bytes
// the backup heap is locates at the start of exp pack memory
copy_actors:
	// set up all values we need
	// avoid t1, t0, a0, a1 and a2 
	// because memcpy uses those
	la t4, ACTOR_HEAP_START
	la t5, $01 // -1
	la t6, ACTOR_SEARCH_VALUE
	la t7, $04 // word size
	la a1, ACTOR_HEAP_CLONE
	la v0, {ACTOR_HEAP_SIZE} // loop counter

	// search heap by the word 
copy_actor_loop:
	// next address
	addu t4, t4, t7
	lw t0, $00(t4) 

	subu v0, v0, t5
	beq v0, r0, copy_actor_done // if end address is reached quit 
	nop 

	// check search value 
	bne t0, t6, copy_actor_loop
	nop
	// if loop falls through do a memcpy from a4 - $20 for ACTOR_SIZE 
	// also write original address to A1 first and inc A1
	// set up a0 src ptr
	la t1, ACTOR_OFFSET // offset start
	subu a0, t4, t1 // src ptr 

	// store src ptr
	sw a0, $00(a1)
	addu a1, a1, t7 // next dest ptr

	la a2, ACTOR_SIZE
	// memcpy increments a1 so we do not have to do it here
	// hold return address in v1 for now 
	move v1, ra
	jal memcpy
	nop 
	move ra, v1
	j copy_actor_loop
	nop
copy_actor_done:

	// end list with 1 word of $00
	sw r0, $00(a1)
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
	la a2, ACTOR_SIZE
	jal memcpy 
	nop
	move ra, t7 
	j restore_actor_loop
	nop
restore_actor_done:
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


section_data:
text:
  db "Glove is love!", $00
align($04)
return:
  dw $8013F378

