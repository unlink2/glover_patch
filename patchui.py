#!/usr/bin/env python3

import sys
import glovepatch
import pathlib


if __name__ == '__main__':
    pathlib.Path("./rom/").mkdir(parents=True, exist_ok=True)
    if len(sys.argv) < 2:
        print("Usage: patchui.py <glover rom>")
        exit(1)

    glovepatch.patch_rom(sys.argv[1], './bin/payload.bin', './bin/code.bin', './bin/entry.bin', './rom/glover_patched.z64')
