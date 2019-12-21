arch n64.cpu
endian msb
output "jmp.bin", create

include "lib/N64.INC"

origin $00000000
base $8013F370
Start:
	lui ra, $B078
	jr ra
  
