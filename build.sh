#!/bin/sh
cargo build -Z build-std=core --target linker/mips-none-eabi.json
