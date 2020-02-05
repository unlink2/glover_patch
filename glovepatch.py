#!/usr/bin/env python3

import sys

import cksm
from pathlib import Path

if len(sys.argv) < 5:
    print("Usage: glovepatch.py <glover_rom> <binary to inject> <c code binary> <output>")
    sys.exit(0)


rom = list(Path(sys.argv[1]).read_bytes())
payload = list(Path(sys.argv[2]).read_bytes())
c_code = list(Path(sys.argv[3]).read_bytes())

jump = [0x3C, 0x1F, 0xB0, 0x78, 0x03, 0xE0, 0xF8, 0x09] # code that jumps to payload
jump_address = 0x40370
render_inject = 0x3F844
payload_address = 0x780000
c_code_address = 0x790000
force_easy_ball = 0x3A6D0 # nop this address to always make ball behave like easy mode

for i in range(0, len(jump)):
    rom[jump_address+i] = jump[i]
    rom[render_inject+i] = jump[i]

# need to nop the call right after render_inject
rom[render_inject+8] = 0x00
rom[render_inject+9] = 0x00
rom[render_inject+10] = 0x00
rom[render_inject+11] = 0x00

for i in range(0, len(payload)):
    rom[payload_address+i] = payload[i]

for i in range(0, len(c_code)):
    rom[c_code_address+i] = c_code[i]

# same as this gs code:
# 801396D0 0000
# 801396D1 0000
# 801396D2 0000
# 801396D3 0000
rom[force_easy_ball] = 0x00
rom[force_easy_ball+1] = 0x00
rom[force_easy_ball+2] = 0x00
rom[force_easy_ball+3] = 0x00

buf = bytearray(rom)
cksm.update_checksum(buf)

f = open(sys.argv[4], 'w+b')
f.write(buf)
f.close()
