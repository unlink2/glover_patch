#!/usr/bin/env python3

# this script reads an obj bank dump
# and reports the size of each object
# in a readable way

import sys
import struct

if len(sys.argv) < 2:
    print('usage: obj_bank_size.py <path>')
    sys.exit(1)

content = None
with open(sys.argv[1], mode='rb') as file:
        content = file.read()

SIZE = 20 # struct size
for i in range(0, len(content), 20):
    print(content[i:i+12].decode('utf-8'), hex(struct.unpack_from('>I', content, i+16)[0]))
