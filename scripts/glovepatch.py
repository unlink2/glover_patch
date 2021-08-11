#!/usr/bin/env python3

import sys

import cksm
from pathlib import Path

VIRTUAL_TO_ROM = 0x800ff000

def patch_rom(rom_path, payload_path, c_code_path, entry_code_path, out_path):
    rom = list(Path(rom_path).read_bytes())
    payload = list(Path(payload_path).read_bytes())
    c_code = list(Path(c_code_path).read_bytes())
    entry_code = list(Path(entry_code_path).read_bytes())

    jump = [0x3C, 0x1F, 0xB0, 0x78, 0x03, 0xE0, 0xF8, 0x09] # code that jumps to payload
    jump_ram = [0x3C, 0x1F, 0x80, 0x40, 0x03, 0xE0, 0xF8, 0x09]
    entry_inject = 0x1000
    jump_address = 0x40370 # 0x8013F370
    payload_address = 0x780000
    c_code_address = 0x780200
    force_easy_ball = 0x3A6D0 # nop this address to always make ball behave like easy mode

    for i in range(0, len(jump)):
        rom[jump_address+i] = jump_ram[i]

    for i in range(0, len(payload)):
        rom[payload_address+i] = payload[i]

    for i in range(0, len(c_code)):
        rom[c_code_address+i] = c_code[i]

    for i in range(0, len(entry_code)):
        rom[entry_inject+i] = entry_code[i]


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

    f = open(out_path, 'w+b')
    f.write(buf)
    f.close()

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print("Usage: glovepatch.py <glover_rom> <binary to inject> <c code binary> <entry code> <output>")
        sys.exit(0)
    patch_rom(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
