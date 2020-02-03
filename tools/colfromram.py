#!/usr/bin/env python3

# this script takes a ram dump and attempts
# to find and dump all obj meshes to a waveform

import sys
import struct
from pathlib import Path
import colwav
import os

RDRAM_START = 0x80000000 # base address of ram
RDRAM_END = 0x80400000 # after this we only have expansion ram, no need to search
OBJ_BANK = 0x8025D698 # start of obj_bank
OBJ_BANK_END = 0x80260008

def ram_addr_to_real(addr):
    return addr-RDRAM_START

def search_ram(data, outdir):
    level_data = []
    face_index = 0
    for addr in range(OBJ_BANK, OBJ_BANK_END, 0x14):
        real_offset = ram_addr_to_real(addr)
        type_str = bytearray(data[real_offset:real_offset+0xC]).decode('utf-8')
        # if we found an object start converting
        if type_str.startswith('Obj_Planes'):
            # get pointer
            ptr = struct.unpack('>I', data[real_offset+0xC:real_offset+0x10])
            ptr_real = ram_addr_to_real(ptr[0])
            # get size in bytes
            size = struct.unpack('>I', data[real_offset+0x10:real_offset+0x14])[0]
            # print(hex(ptr[0]), hex(size))

            # get data from ptr
            raw = data[ptr_real:ptr_real+size]
            obj, rawobj = colwav.convert_to_obj(raw)
            # write obj to out dir
            p = open(os.path.join(outdir, str(hex(ptr[0])) + '.obj'), 'w')
            for line in obj:
                p.write(line)
                p.write('\n')

            p.close()

            obj, rawobj = colwav.convert_to_obj(raw, face_start_index=face_index)

            # count vertex
            for line in obj:
                if line.startswith('v'):
                    face_index += 1

            # adjusted data

            # save vert and faces
            level_data.append(obj)

    # combine all vertex arrays and then fix faces accordingly
    # then write all to file
    m = open(os.path.join(outdir, 'merged.obj'), 'w')
    face_data = []
    for data in level_data:
        for line in data:
            if line.startswith('v'):
                m.write(line)
                m.write('\n')
            elif line.startswith('f'):
                face_data.append(line)
    # the last face always causes issues so we just drop it
    for data in face_data:
        m.write(data)
        m.write('\n')

    m.close()



if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('usage: colfromram.py <ram dump file> <output directory>')
        sys.exit(1)

    raw = Path(sys.argv[1]).read_bytes()
    search_ram(raw, sys.argv[2])
