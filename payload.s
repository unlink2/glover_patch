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
constant START_INPUT($1C)

constant INJECTED_RAM_HI($8000)
constant X_BAC($02B0)
constant Z_BAC($02B4)
constant Y_BAC($02B8)

constant GLOVER_XYZ_HI($8029)
constant GLOVER_X_LO($030C)
constant GLOVER_Z_LO($0310)
constant GLOVER_Y_LO($0314)

constant BALL_XYZ_HI($802A)
constant BALL_X_LO($F9AC)
constant BALL_Z_LO($F9B0)
constant BALL_Y_LO($F9B4)

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

start:
	// jal $80178E98 // call the function we are replacing
	nop

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
not_CR:
not_start:

	// return
	lui t3, $B078 // set up direct load for return address
	lw ra, return(t3) // load it 
	jr ra // return
	nop // need a nop after jr
text:
  db "Hello World!"
return:
  dw $8013F378
