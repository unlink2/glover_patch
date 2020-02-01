#!/usr/bin/env python3

import sys
from pathlib import Path
import struct

# this script converts a ram dump
# of collision data
# each actor/object has a ptr to collision data at E0
# each polygon is 30 words in size
# the size for collision is read at 0x8015b7e8 - 0x8015b7f8 break there to find size
# TODO there is lots of unknown data points in each polygon


def convert_to_obj(data):
    format_str = '>' + ('f' * int(len(data)/4))
    unpacked = struct.unpack_from(format_str, data)

    obj = []
    # points per face
    ppf = 3

    for i in range(0, len(unpacked), 30):
        scale = unpacked[i]
        # obj.append('v {0} {1} {2}'.format(unpacked[i+1], unpacked[i+2], unpacked[i+3]))
        obj.append('v {0} {1} {2}'.format(unpacked[i+4], unpacked[i+5], unpacked[i+6]))
        obj.append('v {0} {1} {2}'.format(unpacked[i+7], unpacked[i+8], unpacked[i+9]))
        obj.append('v {0} {1} {2}'.format(unpacked[i+10], unpacked[i+11], unpacked[i+12]))
        # obj.append('v {0} {1} {2}'.format(unpacked[i+13], unpacked[i+14], unpacked[i+15]))
        # obj.append('v {0} {1} {2}'.format(unpacked[i+16], unpacked[i+17], unpacked[i+18]))
        # obj.append('v {0} {1} {2}'.format(unpacked[i+19], unpacked[i+20], unpacked[i+21]))

    for i in range(1, len(obj), ppf):
        face = 'f'
        for j in range(0, ppf):
            face += ' {0}'.format(i+j)
        obj.append(face)

    return obj

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: colwav.py <input data>')
        sys.exit(1)

    raw = Path(sys.argv[1]).read_bytes()

    obj = convert_to_obj(raw)

    for line in obj:
        print(line)
