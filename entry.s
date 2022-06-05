.create "./target/mips-bin/entry.bin", 0xB0780000
.n64

.org 0xB0780000

DMA_RAM equ 0x80400000
DMA_ROM equ 0xB0780000
DMA_SIZE equ 0x9FFFF

// custom entry point

__entry:
    lui sp, 0x8026
    addiu sp, sp, -0xEA8
    lui t0, 0x801F
    addiu t0, t0, 0x5680
    lui t1, 0x802B
    addiu t1, t1, 0x0D10
    beq t0, t1, skip_t0t1
    lui t2, 0x8000
loop:
    addiu t0, t0, 0x0004
    sltu at, t0, t1
    bnez at, loop
skip_t0t1:
    sw r0, 0xFFFC(t0)

    // do dma
    la v0, 0xA4600000
    la v1, ((DMA_RAM) & 0x00FFFFFF)
    sw v1, 0x0000(v0)
    la v1, ((DMA_ROM) & 0x1FFFFFFF)
    sw v1, 0x0004(v0)
    la v1, ((DMA_SIZE))
    sw v1, 0x000C(v0)
dma_loop:
    lw v1, 0x0010(v0)
    andi v1, v1, 0x0001
    bnez v1, dma_loop
    nop

    la v1, 0x00000002
    sw v1, 0x0010(v0)

    ori t3, r0, 0x0001
    jal 0x80139DE8
    sw  t3, 0x0300(t2)

.close
