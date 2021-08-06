#!/bin/sh

# this file simply calls commands to
# build with the mips-target
# and it generates a dump
# of the binary

./build.sh
./dump.sh

mkdir -p target/mips-bin/
mkdir -p rom/
armips entry.s -sym ./target/mips-bin/entry.sym
armips payload.s -sym ./target/mips-bin/payload.sym
python3 ./scripts/glovepatch.py ./rom/glover_base_ntsc.z64 target/mips-bin/payload.bin target/mips-none-eabi/debug/payload.bin target/mips-bin/entry.bin ./rom/glover_patched.z64
