#include "include/logic.h"
#include "include/inputs.h"
#include "include/memwatch.h"
#include "include/debug.h"
#include "include/menu.h"

// x y and z coordinates
WORD_T gpos_bac[3];
// camera backup
WORD_T gcam_bac[CAMERA_ACTOR_SIZE];

gpatch_t gpatch;

void logic() {
    update_memwatch(&pmemwatch);
    update_menu(&pmenu);

    // test for infinites
    if (gpatch.infinite_hp) {
        get_ptr(WORD_T, hp, GLOVER_HP, 1);
        *hp = 0xFF;
    }
    if (gpatch.infinite_lives) {
        get_ptr(WORD_T, lives, GLOVER_LIVES, 1);
        *lives = 0xFF;
    }

    // only trigger this code if start is held
    if (read_button(START_INPUT, CONTROLLER_1)) {
        if (read_button(A_INPUT, CONTROLLER_1)) {
            enable_timer();
        }
        if (read_button(B_INPUT, CONTROLLER_1)) {
            level_select();
        }

        if (read_button(CL_INPUT, CONTROLLER_1)) {
            store_glover_pos();
        }

        if (read_button(CR_INPUT, CONTROLLER_1)) {
            restore_glover_pos();
        }

        if (read_button(CU_INPUT, CONTROLLER_1)
                && !read_button(CU_INPUT, LAST_INPUT_1)) {
            clone_actors();
            // evd_write_msg(0x21);
        }

        if (read_button(CD_INPUT, CONTROLLER_1)
                && !read_button(CD_INPUT, LAST_INPUT_1)) {
            restore_actors();
            // evd_init();
        }
    }

    // controller 2

    if (read_button(CU_INPUT, CONTROLLER_2)
        && !read_button(CU_INPUT, LAST_INPUT_2)) {
        get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE, 1);
        *pframe_advance = 2; // set to 2 to prevent double input

        // store last input values
        store_inputs(CONTROLLER_2, LAST_INPUT_2);
    }

    frame_advance();

    get_ptr(WORD_T, file1, FILE1_START, 7);
    complete_file(file1);

    // store last input values
    store_inputs(CONTROLLER_2, LAST_INPUT_2);
    store_inputs(CONTROLLER_1, LAST_INPUT_1);
}

void enable_timer() {
    get_ptr(HWORD_T, ptr, TIMER_HW, 1);
    (*ptr) = 0xFF;
}

void level_select() {
    get_ptr(BYTE_T, pgamemode_ptr, GAME_MODE, 1);
    get_ptr(BYTE_T, ppause_ptr, PAUSE_FLAG, 1);
    *pgamemode_ptr = 0x02; // enable level select
    *ppause_ptr = 0x00; // disable pause
}

void frame_advance() {
    get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE, 1);
    // get_ptr(WORD_T, plast_z_write, 0x801349B4, 1);

    while (*pframe_advance) {
        if (read_button(CD_INPUT, CONTROLLER_2)
                && *pframe_advance == 1) {
            *pframe_advance = 2;
            // restore instruction
            // *plast_z_write = 0xA0440000; // original instruction
            break;
        } else if (!read_button(CD_INPUT, CONTROLLER_2)
                && !read_button(CU_INPUT, CONTROLLER_2)) {
            *pframe_advance = 1;
        } else if(read_button(CU_INPUT, CONTROLLER_2)
                && *pframe_advance == 1) {
            *pframe_advance = 0;
        }
        store_inputs(CONTROLLER_2, LAST_INPUT_2);

        // patch out bad code we dont want
        // this code prevents Z inputs from working correctly
        // replace with nop
        // *plast_z_write = 0x00; // nop
    }
}

void complete_file(WORD_T *pfile) {
    // read level completion
    int completed = pfile[1];
    if (completed != 0xFFFFFFFF) {
        // write whole file
        pfile[0] = 0x4B4C4D00; // Filename

        // clearled levels
        pfile[1] = 0xFFFFFFFF;
        pfile[2] = 0xFFFFFFFF;

        // score
        pfile[3] = 0x00000190;

        // difficulty and castle progress
        pfile[4] = 0x0A000606;

        // unknown
        pfile[5] = 0x40000001;

        // sound options etc
        pfile[6] = 0x006E006E;
    }
}

void store_glover_pos() {
    WORD_T *pgloverx = GLOVER_X;
    WORD_T *pglovery = GLOVER_Y;
    WORD_T *pgloverz = GLOVER_Z;

    get_ptr(WORD_T, pcam, CAMERA_ACTOR, CAMERA_ACTOR_SIZE);


    gpos_bac[0] = *pgloverx;
    gpos_bac[1] = *pglovery;
    gpos_bac[2] = *pgloverz;

    gmemcpy((BYTE_T*)pcam, (BYTE_T*)gcam_bac, CAMERA_ACTOR_SIZE);
}

void restore_glover_pos() {
    WORD_T *pgloverx = GLOVER_X;
    WORD_T *pglovery = GLOVER_Y;
    WORD_T *pgloverz = GLOVER_Z;
    WORD_T *pballx = BALL_X;
    WORD_T *pbally = BALL_Y;
    WORD_T *pballz = BALL_Z;

    get_ptr(WORD_T, pcam, CAMERA_ACTOR, CAMERA_ACTOR_SIZE);

    // stop glover
    WORD_T *pglovervelx = GLOVER_VEL_X;
    WORD_T *pglovervely = GLOVER_VEL_Y;
    WORD_T *pglovervelz = GLOVER_VEL_Z;
    *pglovervelx = 0;
    *pglovervely = 0;
    *pglovervelz = 0;

    *pgloverx = gpos_bac[0];
    *pballx = gpos_bac[0];
    *pglovery = gpos_bac[1];
    *pbally = gpos_bac[1];
    *pgloverz = gpos_bac[2];
    *pballz = gpos_bac[2];

    gmemcpy((BYTE_T*)gcam_bac, (BYTE_T*)pcam, CAMERA_ACTOR_SIZE);
}

WORD_T* clone_additional(WORD_T *src, WORD_T *pcloneptr, WORD_T size) {
    if (src == NULL) {
        return pcloneptr;
    }

    // memcopy if not NULL
    *pcloneptr = (WORD_T)src;
    pcloneptr += 1;
    *pcloneptr = size; // bytes
    pcloneptr += 1;
    gmemcpy((BYTE_T*)src, (BYTE_T*)pcloneptr, size);
    pcloneptr += size/4;

    return pcloneptr;
}

void clone_actors() {
    // actor value here is also the actor's next ptr
    // actor heap loops on itself
    // if we're back at the start we are done
    get_ptr(WORD_T, pactor, ACTOR_HEAP_START, 1);
    get_ptr(WORD_T, pcloneptr, ACTOR_HEAP_CLONE, 1); // current clone address

    do {
        *pcloneptr = (WORD_T)pactor;
        pcloneptr += 1; // next word
        *pcloneptr = ACTOR_SIZE;
        pcloneptr += 1;

        // clone here
        gmemcpy((BYTE_T*)pactor, (BYTE_T*)pcloneptr, ACTOR_SIZE);

        // + F0
        pcloneptr += ACTOR_SIZE/4;

        // TODO clones crash often, may need more values
        // clone actor support values if applicable
        // 0x50 bytes ptr found at pactor + C8 (50 words)
        // WORD_T *psupport = (WORD_T*)*(pactor + 0xC8/4);
        // pcloneptr = clone_additional(psupport, pcloneptr, 0x50);

        // clone animation state
        // 0x50 bytes per actor if not NULL
        // WORD_T *panimation = (WORD_T*)*(pactor + 0xDC/4);
        // pcloneptr = clone_additional(panimation, pcloneptr, 0x50);

        pactor = (WORD_T*)(*pactor); // next value
    } while (pactor != ACTOR_HEAP_START);
    // pcloneptr += 1;
    // clone camera
    get_ptr(WORD_T, pcam, CAMERA_ACTOR, CAMERA_ACTOR_SIZE);
    pcloneptr = clone_additional(pcam, pcloneptr, CAMERA_ACTOR_SIZE);
    /**pcloneptr = (WORD_T)pcam;
    pcloneptr += 1; // next word
    *pcloneptr = CAMERA_ACTOR_SIZE;
    pcloneptr += 1;
    gmemcpy((BYTE_T*)pcam, (BYTE_T*)pcloneptr, CAMERA_ACTOR_SIZE);
    pcloneptr += CAMERA_ACTOR_SIZE/4;*/

    // finish list with 00
    // pcloneptr += 1;
    *pcloneptr = 0x00;

    get_ptr(WORD_T, prng, RNG_VALUE, 1)
    // store rng value
    pcloneptr += 1;
    *pcloneptr = *prng;
}

void restore_actors() {
    WORD_T *pactor = NULL;
    get_ptr(WORD_T, pcloneptr, ACTOR_HEAP_CLONE, 1);  // current clone address

    do {
        pactor = (WORD_T*)*pcloneptr;
        // size value
        pcloneptr += 1;
        WORD_T size = *pcloneptr;
        pcloneptr += 1;

        gmemcpy((BYTE_T*)pcloneptr, (BYTE_T*)pactor, size);
        pcloneptr += size/4; // next value
    } while (*pcloneptr != 0x00);

    get_ptr(WORD_T, prng, RNG_VALUE, 1);
    // restore rng
    pcloneptr += 1;
    *prng = *pcloneptr;
}

void toggle_collision() {
    get_ptr(WORD_T, pcol, COLLISION_DISABLE, 1);
    if (*pcol == 0x00000000) {
        *pcol = 0x0C057828; // restore code
    } else {
        *pcol = 0x000000000; // nop
    }
}

void toggle_fog() {
    get_ptr(BYTE_T, pfog, FOG, 1);
    *pfog ^= 1;
}
