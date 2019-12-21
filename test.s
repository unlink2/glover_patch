arch n64.cpu
endian msb
output "payload.bin", create

origin $00000000
base $B0780000

include "lib/N64.INC" // Include N64 Definitions

constant INPUTS_HI($BFC0)
constant INPUTS_LO($07C4)

constant L_INPUT($00002000)
constant R_INPUT($00001000)
constant CL_INPUT($00000200)
constant CR_INPUT($00000100)

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

Start:
	// jal $80178E98 // call the function we are replacing
	nop

	lui t3, INPUTS_HI // inputs bit 
	lw t1, INPUTS_LO(t3)

	srl t1, t1, $08 // shift 4 bits to have an easier time anding

	// check L press 
	andi t1, t1, CL_INPUT
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

	lui t3, INPUTS_HI // inputs bit 
	lw t1, INPUTS_LO(t3)

	srl t1, t1, $08 // shift 4 bits to have an easier time anding

	// check R press 
	andi t1, t1, CR_INPUT
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

	// return
	lui t3, $B078 // set up direct load for return address
	lw ra, Return(t3) // load it 
	jr ra // return
	nop // need a nop after jr??
Text:
  db "Hello World!"
Return:
  dw $8013F378
