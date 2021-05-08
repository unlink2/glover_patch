arch n64.cpu
endian msb
output "./target/mips-bin/entry.bin", create

origin $00000000
base $80100000

include "lib/N64.INC" // Include N64 Definitions


constant DMA_RAM($80400000)
constant DMA_ROM($B0780000)
constant DMA_SIZE($1FFFF)

// custom entry point

__entry:
    lui sp, $8026
    addiu sp, sp, -$EA8
    lui t0, $801F
    addiu t0, t0, $5680
    lui t1, $802B
    addiu t1, t1, $0D10
    beq t0, t1, skip_t0t1
    lui t2, $8000
loop:
    addiu t0, t0, $0004
    sltu at, t0, t1
    bnez at, loop
skip_t0t1:
    sw r0, $FFFC(t0)

    // do dma
    la v0, $A4600000
    la v1, ((DMA_RAM) & $00FFFFFF)
    sw v1, $0000(v0)
    la v1, ((DMA_ROM) & $1FFFFFFF)
    sw v1, $0004(v0)
    la v1, ((DMA_SIZE))
    sw v1, $000C(v0)
dma_loop:
    lw v1, $0010(v0)
    andi v1, v1, 0x0001
    bnez v1, dma_loop
    nop

    la v1, $00000002
    sw v1, $0010(v0)

    ori t3, r0, $0001
    jal $80139DE8
    sw  t3, $0300(t2)

