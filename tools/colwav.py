#!/usr/bin/env python3

import sys
from pathlib import Path
import struct

# this script converts a ram dump
# of collision data
# each actor/object has a ptr to collision data at E0
# each polygon is 30 words in size
# the size for collision is read at 0x8015b7e8 - 0x8015b7f8 break there to find size
# to get size:
# follow ptr ar  C8 in actor/object from that ptr follow 0C from that ptr follow 18 to get to model or 14
# to get a half word for size (amount of faces?) of collision
# TODO there is lots of unknown data points in each polygon

# data = raw collision data dumped from game
# size = total size of collision data, if none given len(data) is used
# node_size = size for each vertex
# tri_offset = start of actual tri data in vertex node
# returns an object, raw BE encoded floats, vertex list and a tuple of face string and face index
def convert_to_obj(data, size=None, node_size=30, tri_offset=4, face_start_index=0):
    if size is None:
        size = len(data)

    format_str = '>' + ('f' * int((size/4)))
    unpacked = struct.unpack_from(format_str, data)

    obj = []
    raw = []
    # points per face
    ppf = 3

    for i in range(0, len(unpacked), node_size):
        scale = unpacked[i]
        x = tri_offset+i
        # obj.append('v {0} {1} {2}'.format(unpacked[i+1], unpacked[i+2], unpacked[i+3]))
        try:
            obj.append('v {0} {1} {2}'.format(unpacked[x], unpacked[x+1], unpacked[x+2]))
            obj.append('v {0} {1} {2}'.format(unpacked[x+3], unpacked[x+4], unpacked[x+5]))
            obj.append('v {0} {1} {2}'.format(unpacked[x+6], unpacked[x+7], unpacked[x+8]))
            # vert.append('v {0} {1} {2}'.format(unpacked[x], unpacked[x+1], unpacked[x+2]))
            # vert.append('v {0} {1} {2}'.format(unpacked[x+3], unpacked[x+4], unpacked[x+5]))
            # vert.append('v {0} {1} {2}'.format(unpacked[x+6], unpacked[x+7], unpacked[x+8]))
            for j in range (i+4, i+13):
                raw.append(struct.pack('>f', unpacked[j]))
        except IndexError:
            break
        # obj.append('v {0} {1} {2}'.format(unpacked[i+13], unpacked[i+14], unpacked[i+15]))
        # obj.append('v {0} {1} {2}'.format(unpacked[i+16], unpacked[i+17], unpacked[i+18]))
        # obj.append('v {0} {1} {2}'.format(unpacked[i+19], unpacked[i+20], unpacked[i+21]))

    for i in range(face_start_index+1, face_start_index+len(obj), ppf):
        face = 'f'
        fd = []
        for j in range(0, ppf):
            face += ' {0}'.format(i+j)
            fd.append(i+j)
        obj.append(face)
        # faces.append(face)

    return obj, raw

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: colwav.py <input data> [raw_file]')
        sys.exit(1)

    raw = Path(sys.argv[1]).read_bytes()

    obj, convraw = convert_to_obj(raw)

    if len(sys.argv) >= 3:
        f = open(sys.argv[2], 'wb')
        for ba in convraw:
            f.write(ba)
        f.close()

    for line in obj:
        print(line)
