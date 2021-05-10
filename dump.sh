#!/bin/sh

target="debug"
if [ -z "$1" ]
then
    target="debug"
else
    target=$1
fi


rust-objcopy -O binary --only-section=.text --only-section=.rodata --only-section=.bss --only-section=.data --only-section=.boot "./target/mips-none-eabi/$target/payload" "./target/mips-none-eabi/$target/payload.bin"
rust-objdump -S "target/mips-none-eabi/$target/payload" > "target/mips-none-eabi/$target/payload.sym"
# rust-size "target/mips-none-eabi/$target/payload"
# rust-objdump -h "target/mips-none-eabi/$target/payload"
# rust-objdump -Ct "target/mips-none-eabi/$target/payload"
