#!/usr/bin/env python3

from pathlib import Path
import sys
import struct

# takes a file exported using steps in bitmap_format.txt

width = 320
height = 240
bpp = 16

if len(sys.argv) < 3:
    print("Usage ./addbmpheader.py <source> <dest> [width] [height] [bpp] [rotate right] [true/false flip]")
    sys.exit(1)

if len(sys.argv) >= 4:
    width = int(sys.argv[3])
if len(sys.argv) >= 5:
    height = int(sys.argv[4])

rotate_by = 1
flip = True
if len(sys.argv) >= 7:
    rotate_by = int(sys.argv[6])

if len(sys.argv) >= 8:
    if sys.argv[7] == 'true':
        flip = True
    else:
        flip = False

if len(sys.argv) >= 6:
    bpp = int(sys.argv[5])

raw = list(Path(sys.argv[1]).read_bytes())

if flip:
    # image is upside down and all bytes are flipped
    raw.reverse()

    # rotate rows to get right horizontal flip
    raw = [raw[x:x+width*2] for x in range(0, len(raw), width*2)]
    newraw = []

    for r in raw:
        r.reverse()
        newraw += r
    raw = newraw


if rotate_by > 0:
    # to list of lists, 1 list per pixel
    raw = [raw[x:x+2] for x in range(0, len(raw), 2)]

    # Rotate right: 0b1001 --> 0b1100
    ror = lambda val, r_bits, max_bits: \
                ((val & (2**max_bits-1)) >> r_bits%max_bits) | \
                    (val << (max_bits-(r_bits%max_bits)) & (2**max_bits-1))

    # Rotate left: 0b1001 --> 0b0011
    rol = lambda val, r_bits, max_bits: \
                (val << r_bits%max_bits) & (2**max_bits-1) | \
                    ((val & (2**max_bits-1)) >> (max_bits-(r_bits%max_bits)))

    newraw = []
    # flatten and adjust bits in list
    for i in raw:
        int_value = struct.unpack("<H", bytearray([i[0], i[1]]))
        # rotate to move colors to correct position
        int_value = ror(int_value[0], rotate_by, 16)
        byte_value = struct.pack("<H", int_value)
        newraw.append(byte_value[0])
        newraw.append(byte_value[1])

    raw = newraw


HEADER_SIZE = 54 # 54 bytes for header

def append_int(n, header):
    file_size = struct.pack('<I', n)
    # add file size (4 bytes)
    for i in range(0, 4):
        try:
            header.append(file_size[i])
        except IndexError:
            header.append(00)
    return header

# add header
header = []
header.append(0x42) # B
header.append(0x4D) # M

# file size
header = append_int(len(raw)+HEADER_SIZE, header)

# reserved
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# pixel data offset is always 55
header.append(55)
header.append(00)
header.append(00)
header.append(00)

# info header
header = append_int(40, header) # header size

# image width
header = append_int(width, header)
# image height
header = append_int(height, header)

# planes
header.append(1)
header.append(00)

# bits per pixel
header.append(bpp)
header.append(00)

# compression
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# imagesize after compression
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# x pixels per meter
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# y pixels per meter
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# total colors
header.append(00)
header.append(00)
header.append(00)
header.append(00)

# important colors
header.append(00)
header.append(00)
header.append(00)
header.append(00)


buf = bytearray(header + raw)
f = open(sys.argv[2], 'w+b')
f.write(buf)
f.close()
