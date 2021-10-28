#!/bin/sh
cargo build -Z build-std=core,alloc --target linker/mips-none-eabi.json
