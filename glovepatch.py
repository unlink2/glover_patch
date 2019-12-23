#!/usr/bin/env python3

import sys

from pathlib import Path

if len(sys.argv) < 4:
    print("Usage: glovepatch.py <glover_rom> <binary to inject> <output>")
    sys.exit(0)


rom = list(Path(sys.argv[1]).read_bytes())
payload = list(Path(sys.argv[2]).read_bytes())
jump = [0x3C, 0x1F, 0xB0, 0x78, 0x03, 0xE0, 0x00, 0x08] # code that jumps to payload 
jump_address = 0x40370
payload_address = 0x780000
force_easy_ball = 0x3A6D0 # nop this address to always make ball behave like easy mode

for i in range(0, len(jump)):
    rom[jump_address+i] = jump[i]

for i in range(0, len(payload)):
    rom[payload_address+i] = payload[i]

rom[force_easy_ball] = 0x00
rom[force_easy_ball+1] = 0x00
rom[force_easy_ball+2] = 0x00
rom[force_easy_ball+3] = 0x00

f = open(sys.argv[3], 'w+b')
binary_format = bytearray(rom)
f.write(binary_format)
f.close()
