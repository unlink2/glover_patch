#!/bin/sh

# this file simply calls commands to
# build with the mips-target
# and it generates a dump
# of the binary

./build.sh
./dump.sh

mkdir -p target/mips-bin/
mkdir -p rom/
bass entry.s -sym ./target/mips-bin/entry.sym
python3 ./scripts/glovepatch.py ./rom/glover_base_ntsc.z64 target/mips-none-eabi/debug/payload.bin target/mips-bin/entry.bin ./rom/glover_patched.z64
