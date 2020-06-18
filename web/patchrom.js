const logger = new justlog.JustLog('GloverPatch', justlog.JustLog.LEVEL_DEBUG,
    new justlog.JustLogConsoleStream({color: false}));

/**
 * @typedef {Object} Settings
 * @property {Bool} patchEasy Should easy mode be patched int?
 */


/**
 * All CRCs I could find
 */
const SOURCE_CRC = {
    '!': [ -1905393153, -860554936 ],
    'b1': [ 239407100, -1610841227 ],
    'b2': [ 2004083387, 1909500197 ],
    't1': [ 1368621458, -1119184735 ],
    /* PAL CRCs */
    /*'e!': [ -182226175, -1713115501 ],
    'ef1': [ 1294684952, 1205555990 ]*/}
const PAYLOAD_PATH = './bin/payload.bin';
const CODE_PATH = './bin/code.bin';
const ENTRY_PATH = './bin/entry.bin';
const VIRTUAL_TO_ROM = 0x800ff000;

/**
 * @function patch Patches the rom
 * @param {DataView} rom The rom data
 * @param {Settings} settings Patcher settings object
 */
let patch = async (rom, settings, payload_path=PAYLOAD_PATH, code_path=CODE_PATH, entry_path=ENTRY_PATH) => {
    const payload_res = await fetch(payload_path);
    const ccode_res = await fetch(code_path);
    const entry_res = await fetch(entry_path);
    let payload = new DataView(await payload_res.arrayBuffer());
    let c_code = new DataView(await ccode_res.arrayBuffer());
    let entry_code = new DataView(await entry_res.arrayBuffer());

    let jump = [0x3C, 0x1F, 0xB0, 0x78, 0x03, 0xE0, 0xF8, 0x09]; // code that jumps to payload
    let jump_ram = [0x3C, 0x1F, 0x80, 0x40, 0x03, 0xE0, 0xF8, 0x09];
    let entry_inject = 0x1000;
    let jump_address = 0x40370;
    let render_inject = 0x8017FF10-VIRTUAL_TO_ROM;
    let payload_address = 0x780000;
    let c_code_address = 0x780200;
    let force_easy_ball = 0x3A6D0 // nop this address to always make ball behave like easy mode

    for (let i = 0; i < jump.length; i++) {
        rom.setUint8(jump_address+i, jump_ram[i]);
        rom.setUint8(render_inject+i, jump_ram[i]);
    }

    // need to nop the call right after render_inject
    for (let i = 8; i < 12; i++) {
        rom.setUint8(render_inject+i, 0x00);
    }

    for (let i = 0; i < payload.buffer.byteLength; i++) {
        rom.setUint8(payload_address+i, payload.getUint8(i));
    }

    for (let i = 0; i < c_code.buffer.byteLength; i++) {
        rom.setUint8(c_code_address+i, c_code.getUint8(i));
    }

    for (let i = 0; i < entry_code.buffer.byteLength; i++) {
        rom.setUint8(entry_inject+i, entry_code.getUint8(i));
    }


    // same as this gs code:
    // 801396D0 0000
    // 801396D1 0000
    // 801396D2 0000
    // 801396D3 0000
    if (settings.shouldPatchEasy) {
        rom.setUint8(force_easy_ball, 0x00);
        rom.setUint8(force_easy_ball+1, 0x00);
        rom.setUint8(force_easy_ball+2, 0x00);
        rom.setUint8(force_easy_ball+3, 0x00);
    }

    return rom;
}


/**
 * Fixes checksum
 * based on python implementation
 */
let crc = async (buf) => {
    let base = 0xffffffff;

    let t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
    let s0, s6;
    let a0, a1, a2, a3, at;
    let v0, v1;

    t0 = t1 = t2 = t3 = t4 = t5 = t6 = t7 = t8 = t9 = 0;
    s0 = s6 = 0;
    a0 = a1 = a2 = a3 = at = 0;
    let lo = 0;
    v0 = v1 = 0;
    let ra = 0;

    // derived from the SM64 boot code
    s6 = 0x3f;
    a0 = 0x1000;     // 59c:   8d640008    lw a0,8(t3)
    a1 = s6;         // 5a0:   02c02825    move  a1,s6
    at = 0x5d588b65; // 5a4:   3c015d58    lui   at,0x5d58
    // 5a8:   34218b65    ori   at,at,0x8b65
    lo = ((a1 * at) & base) >>> 0;    // 5ac:   00a10019    multu a1,at    16 F8CA 4DDB
    ra = 0x100000;   // 5bc:  3c1f0010    lui   ra,0x10
    v1 = 0;          // 5c0:  00001825    move  v1,zero
    t0 = 0;         // 5c4:  00004025    move  t0,zero
    t1 = a0;         // 5c8:  00804825    move  t1,a0
    t5 = 32;         // 5cc:  240d0020    li t5,32
    v0 = lo;         // 5d0:  00001012    mflo  v0
    v0 = ((v0 + 1) & base) >>> 0;         // 5d4:  24420001    addiu v0,v0,1
    a3 = v0;         // 5d8:  00403825    move  a3,v0
    t2 = v0;         // 5dc:  00405025    move  t2,v0
    t3 = v0;         // 5e0:  00405825    move  t3,v0
    s0 = v0;         // 5e4:  00408025    move  s0,v0
    a2 = v0;         // 5e8:  00403025    move  a2,v0
    t4 = v0;         // 5ec:  00406025    move  t4,v0

    while (t0 != ra) {
        v0 = buf.getUint32(t1, false); // 5f0: 8d220000    lw v0,0(t1)
        v1 = ((a3 + v0) & base) >>> 0;   // 5f4: 00e21821    addu  v1,a3,v0
        at = ((v1 < a3)) >>> 0;          // 5f8: 0067082b    sltu  at,v1,a3
        a1 = v1;                 // 600: 00602825    move  a1,v1 branch delay slot

        if (at) {           // 5fc: 10200002    beqz  at,0x608
            t2 = ((t2 + 1) & base) >>> 0; // 604: 254a0001    addiu t2,t2,1
        }

        v1 = (v0 & 0x1F) >>> 0;          // 608: 3043001f    andi  v1,v0,0x1f
        t7 = ((t5 - v1) & base) >>> 0;   // 60c: 01a37823    subu  t7,t5,v1
        t8 = ((v0 >>> t7) & base) >>> 0;           // 610: 01e2c006    srlv  t8,v0,t7
        t6 = ((v0 << v1) & base) >>> 0;  // 614: 00627004    sllv  t6,v0,v1
        a0 = (t6 | t8) >>> 0;            // 618: 01d82025    or a0,t6,t8
        at = ((a2 < v0)) >>> 0;          // 61c: 00c2082b    sltu  at,a2,v0
        a3 = (a1) >>> 0;                 // 620: 00a03825    move  a3,a1
        t3 = ((t3 ^ v0) & base) >>> 0;   // 624: 01625826    xor   t3,t3,v0
        s0 = ((s0 + a0) & base) >>> 0;   // 62c: 02048021    addu  s0,s0,a0 branch delay slot
        if (at) {           // 628: 10200004    beqz  at,0x63c
            t9 = ((a3 ^ v0) & base) >>> 0; // 630: 00e2c826    xor   t9,a3,v0
            // 634: 10000002    b  0x640
            a2 = ((a2 ^ t9) & base) >>> 0; // 638: 03263026    xor   a2,t9,a2 branch delay
        }
        else {
            a2 = ((a2 ^ a0) & base) >>> 0; // 63c: 00c43026    xor   a2,a2,a0
        }
        t0 = ((t0 + 4) & base) >>> 0;                 // 640: 25080004    addiu t0,t0,4
        t7 = ((v0 ^ s0) & base) >>> 0;   // 644: 00507826    xor   t7,v0,s0
        t1 = ((t1 + 4) & base) >>> 0;                 // 648: 25290004    addiu t1,t1,4
        t4 = ((t4 + t7) & base) >>> 0;   // 650: 01ec6021    addu  t4,t7,t4 branch delay
    }
    t6 = (a3 ^ t2) & base;    // 654: 00ea7026    xor   t6,a3,t2
    a3 = (t6 ^ t3) & base;    // 658: 01cb3826    xor   a3,t6,t3
    t8 = (s0 ^ a2) & base;    // 65c: 0206c026    xor   t8,s0,a2
    s0 = (t8 ^ t4) & base;    // 660: 030c8026    xor   s0,t8,t4

    logger.info('CRC: ', a3, s0);
    return [a3, s0]
}

let isCrcOk = (basecrc) => {
    for (let k in SOURCE_CRC) {
        if (SOURCE_CRC[k][0] === basecrc[0]
            && SOURCE_CRC[k][1] === basecrc[1]) {
            if (k != '!' && warning) {
                // warn user about potentially bad ROM
                warning(`Warning: ${k} is a potentially bad dump of the ROM. The hack may not work as expected.`);
            }
            return true;
        }
    }
    return false;
}

let swapHalfWord = (data, index) => {
    let val = data.getUint16(index, false);
    val = ((val & 0xFF) << 8)
               | ((val >> 8) & 0xFF);
    data.setUint16(index, val, false);
}

let swapToZ64 = (data, fileName) => {
    let head = data.getUint32(0, false);
    // check first byte
    if (fileName.endsWith('.n64')) { // n64 format. bad!
        logger.info('Converting to .z64!');
        for (let i = 0; i < data.byteLength; i += 2) {
            // convert every half word
            swapHalfWord(data, i);
        }
    } else {
        logger.info('No need to convert to .z64.');
    }
    return data;
}
