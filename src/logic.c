#include "logic.h"
#include "inputs.h"
#include "memwatch.h"
#include "debug.h"
#include "menu.h"
#include "actor.h"
#include "debug.h"
#include "playerinfo.h"

// x y and z coordinates
float gpos_bac[3];
// camera backup
WORD_T gcam_bac[CAMERA_ACTOR_SIZE];
// map stored +2 for object bank
u8 lastmap[MAX_RESTORE_SLOTS+2];

gpatch_t gpatch;

/*
 * Set message
 */
void notify(gpatch_t *pgpatch, const char *message, u16 timer) {
    pgpatch->message = (char*)message;
    pgpatch->msg_timer = timer;
}

void logic() {
    update_memwatch(&pmemwatch);
    update_menu(&pmenu);
    update_keyboard(&pkb);
    evd_serial_terminal(&pmemwatch);

    // update message
    if (gpatch.msg_timer > 0) {
        gpatch.msg_timer--;
    } else {
        gpatch.message = NULL;
    }

    // TODO hacky way to prevent crash
    // Toggle menu once
    if (gpatch.menu_toggle == 0) {
        pmenu.flags = 0x80;
        gpatch.menu_toggle++;
    } else if (gpatch.menu_toggle == 1) {
        pmenu.flags = 0x00;
        gpatch.menu_toggle++;
    }

    // test for infinites
    if (gpatch.infinite_hp) {
        get_ptr(WORD_T, hp, GLOVER_HP, 1);
        *hp = 0xFF;
    }

    if (gpatch.infinite_lives) {
        get_ptr(WORD_T, lives, GLOVER_LIVES, 1);
        *lives = 0xFF;
    }

    if (gpatch.disable_pause) {
        get_ptr(BYTE_T, nopause, DISABLE_PAUSE_FLAG, 1);
        *nopause = 0x01;
    }

    if (gpatch.lock_pos) {
        restore_glover_pos();
    }
    if (gpatch.infinite_jump) {
        get_ptr(BYTE_T, infjump, INFINITE_DOUBLE_JUMP, 1);
        *infjump = 0;
    }
    if (gpatch.cutscene_skip) {
        get_ptr(BYTE_T, css, DEMO_END_TIMER, 1);
        get_ptr(BYTE_T, gm, GAME_STATE, 1);
        if (*gm != 2) {
            *css = 1; // always set to 1
        }
    }

    get_ptr(u8, current_map, CURRENT_MAP, 1);
    if (*current_map == 0x2C || *current_map == 0x2B) { // intro scene
        get_ptr(BYTE_T, disable_pause, DISABLE_PAUSE_FLAG, 1);
        *disable_pause = 0x01; // prevent pause menu in intro
    }

    // see if restore was interrupted
    if (gpatch.resume_restore) {
        get_ptr(u16, fade_timer, FADE_TIMER, 1);
        if (lastmap[gpatch.restore_slot] == *current_map && *fade_timer == 0) {
            if (gpatch.resume_timer++ > 60) {
                restore_actors(NULL, gpatch.restore_slot);
                gpatch.resume_restore = FALSE;
            }
        }
    }

    update_timer(&gpatch);

    // only trigger this code if start is held
    if (read_button(START_INPUT, CONTROLLER_1)) {
        get_ptr(BYTE_T, pause_anim, PAUSE_MENU_ANIMATION, 1);

        if (read_button(A_INPUT, CONTROLLER_1)
                && !read_button(A_INPUT, LAST_INPUT_1)) {
            // enable_timer();
            toggle_timer(&gpatch);
        }
        if (read_button(B_INPUT, CONTROLLER_1)) {
            level_select();
            *pause_anim = 0x00;
        }

        if (read_button(CL_INPUT, CONTROLLER_1)) {
            store_glover_pos();
        }

        if (read_button(CR_INPUT, CONTROLLER_1)) {
            restore_glover_pos();
        }

        if (read_button(CU_INPUT, CONTROLLER_1)
                && !read_button(CU_INPUT, LAST_INPUT_1)) {
            clone_actors(NULL, gpatch.restore_slot);
            // evd_write_msg(0x21);
        }

        if (read_button(CD_INPUT, CONTROLLER_1)
                && !read_button(CD_INPUT, LAST_INPUT_1)) {
            restore_actors(NULL, gpatch.restore_slot);
            // evd_init();
        }

        if (read_button(Z_INPUT, CONTROLLER_1)
                && !read_button(Z_INPUT, LAST_INPUT_1)) {
            pkb.render_inputs = !pkb.render_inputs;
        }
    }

    // controller 2

    if (read_button(CU_INPUT, CONTROLLER_2)
        && !read_button(CU_INPUT, LAST_INPUT_2)) {
        gpatch.frame_advance = 2; // set to 2 to prevent double input

        // store last input values
        store_inputs(CONTROLLER_2, LAST_INPUT_2);
    }

    // glover force movement
    if (read_button(CL_INPUT, CONTROLLER_2)) {
        get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);

        pglover->ypos += 2.0f;
    }
    if (read_button(CR_INPUT, CONTROLLER_2)) {
        get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);

        pglover->ypos -= 2.0f;
    }



    frame_advance();

    get_ptr(save_file, file1, FILE1_START, 7);
    complete_file(file1);

    // store last input values
    store_inputs(CONTROLLER_2, LAST_INPUT_2);
    store_inputs(CONTROLLER_1, LAST_INPUT_1);
}

// uses the game auto start conditions as igt (less accurate than my solution but now
// it will match igt...)
void update_timer(gpatch_t *pgpatch) {
    // reset timer if requested
    get_ptr(HWORD_T, disinput, DISABLE_INPUT_TIMER, 1);
    get_ptr(HWORD_T, in_goal1, IN_GOAL1, 1);
    get_ptr(HWORD_T, in_goal2, IN_GOAL2, 1);
    get_ptr(HWORD_T, win_level, WIN_LEVEL, 1);
    get_ptr(HWORD_T, did_hit_load, IS_PAUSED, 1);
    if (pgpatch->auto_timer) {
        /*get_ptr(HWORD_T, igt_delay, IGT_WAIT_TARGET, 1);
          get_ptr(WORD_T, frame_counter, FRAME_COUNTER, 1);
          if (*igt_delay * 0x14 > *frame_counter) {
          toggle_timer(pgpatch);
          pgpatch->enable_timer = TRUE;
          }*/
        get_ptr(HWORD_T, fade, LOAD_FADE, 1);
        get_ptr(HWORD_T, target, TARGET_LOAD_FADE, 1);
        // omly reset when target is 0
        if (*fade != 0x00 && *target == 0x00) {
            pgpatch->reset_now = TRUE;
            pgpatch->enable_timer = FALSE;
        }
        if (pgpatch->reset_now && *disinput == 0) {
            // reset timer
            toggle_timer(pgpatch);
            pgpatch->enable_timer = TRUE;
            pgpatch->reset_now = FALSE;
        }
    }
    if (pgpatch->auto_timer && pgpatch->use_igt) {
        get_ptr(WORD_T, igt, IGT, 1);
        pgpatch->timer_frames = *igt;
        pgpatch->enable_timer = TRUE;
    }

    // only inc timer if not paused && in auto mode, not in goal or not win
    if (pgpatch->enable_timer && *disinput < 0xFF && (*did_hit_load == 0 || !pgpatch->use_igt)
            && *in_goal1 == 0 && *in_goal2 == 0 && *win_level == 0) {
    
        u32 timer_seconds = pgpatch->timer_frames / 20;
        u32 timer_minutes = timer_seconds / 60; // 20 frames per second
        timer_seconds = timer_seconds - timer_minutes * 60;
        u32 timer_frames = pgpatch->timer_frames % 20;

        to_decstr(timer_minutes, pgpatch->timer_str, sizeof(u8));

        // add : and maybe leading 0
        u16 len = gstrlen(pgpatch->timer_str);
        pgpatch->timer_str[len] = ':';
        if (timer_seconds < 10) {
            pgpatch->timer_str[len+1] = '0';
            pgpatch->timer_str[len+2] = '\0';
        } else {
            pgpatch->timer_str[len+1] = '\0';
        }
        to_decstr(timer_seconds, pgpatch->timer_str+gstrlen(pgpatch->timer_str), sizeof(u8));

        // add : and maybe leading 0
        len = gstrlen(pgpatch->timer_str);
        pgpatch->timer_str[len] = ':';
        if (timer_frames < 10) {
            pgpatch->timer_str[len+1] = '0';
            pgpatch->timer_str[len+2] = '\0';
        } else {
            pgpatch->timer_str[len+1] = '\0';
        }
        to_decstr(timer_frames, pgpatch->timer_str+gstrlen(pgpatch->timer_str), sizeof(u8));

        pgpatch->timer_frames++;
        notify(pgpatch, pgpatch->timer_str, 20);
    } else if (pgpatch->enable_timer) {
        notify(pgpatch, pgpatch->timer_str, 20);
    }
}

void toggle_timer(gpatch_t *pgpatch) {
    pgpatch->enable_timer = !pgpatch->enable_timer;
    pgpatch->timer_frames = 0;
    pgpatch->message = NULL;
}

void enable_timer() {
    get_ptr(HWORD_T, ptr, TIMER_HW, 1);
    (*ptr) = 0xFF;
}

void level_select() {
    get_ptr(BYTE_T, disable_pause, DISABLE_PAUSE_FLAG, 1);
    get_ptr(HWORD_T, disable_inputs, DISABLE_INPUT_TIMER, 1);
    get_ptr(BYTE_T, pgamemode_ptr, GAME_MODE, 1);
    get_ptr(BYTE_T, ppause_ptr, PAUSE_FLAG, 1);
    *pgamemode_ptr = 0x02; // enable level select
    *ppause_ptr = 0x00; // disable pause
    *disable_inputs = 0x00;
    *disable_pause = 0x00;
}

void frame_advance() {
    u32 *pframe_advance = &gpatch.frame_advance;
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

void complete_file(save_file *pfile) {
    // read level completion
    int completed = pfile->levels_cleared[0];
    if (completed != 0xFFFFFFFF) {
        // write whole file
        pfile->filename[0] = 'K';
        pfile->filename[1] = 'L';
        pfile->filename[2] = 'M';
        pfile->filename[3] = '\0'; // 0x4B4C4D00; // Filename

        // clearled levels
        pfile->levels_cleared[0] = 0xFFFFFFFF;
        pfile->levels_cleared[1] = 0xFFFFFFFF;

        // score
        pfile->score = 0x00000190;

        // difficulty and castle progress
        pfile->unknown_1 = 0x0A;
        pfile->difficulty = 0x00;
        pfile->castle_progress = 0x0606;
        // pfile[4] = 0x0A000606;

        // unknown
        pfile->unknown_2 = 0x40000001;

        // sound options etc
        pfile->settings = 0x006E006E;
    }
}

void store_glover_pos() {
    get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);


    get_ptr(WORD_T, pcam, CAMERA_ACTOR, CAMERA_ACTOR_SIZE);


    gpos_bac[0] = pglover->xpos;
    gpos_bac[1] = pglover->ypos;
    gpos_bac[2] = pglover->zpos;

    gmemcpy((BYTE_T*)pcam, (BYTE_T*)gcam_bac, CAMERA_ACTOR_SIZE);

    notify(&gpatch, "Position saved!", MSG_TIME);
}

void restore_glover_pos() {
    get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);
    get_ptr(glover_actor, pball, BALL_ACTOR, 1);

    get_ptr(WORD_T, pcam, CAMERA_ACTOR, CAMERA_ACTOR_SIZE);

    // stop glover
    pglover->velx = 0;
    pglover->gravity = 0;
    pglover->velz = 0;

    pglover->xpos = gpos_bac[0];
    pball->xpos = gpos_bac[0];
    pglover->ypos = gpos_bac[1];
    pball->ypos = gpos_bac[1];
    pglover->zpos = gpos_bac[2];
    pball->zpos = gpos_bac[2];

    gmemcpy((BYTE_T*)gcam_bac, (BYTE_T*)pcam, CAMERA_ACTOR_SIZE);

    notify(&gpatch, "Position restored!", MSG_TIME);
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

void clone_actors(WORD_T *pcloneptr, u16 slot) {
    // load current map and store it
    get_ptr(u8, current_map, CURRENT_MAP, 1);
    lastmap[slot] = *current_map;

    // actor value here is also the actor's next ptr
    // actor heap loops on itself
    // if we're back at the start we are done
    get_ptr(glover_actor, pactor, ACTOR_HEAP_START, 1);

    if (!pcloneptr) {
        pcloneptr = ACTOR_HEAP_CLONE+ACTOR_HEAP_SIZE*slot;;
    }
    WORD_T *pstart = pcloneptr; // for success message
    // get_ptr(WORD_T, pcloneptr, ACTOR_HEAP_CLONE, 1); // current clone address

    do {
        *pcloneptr = (WORD_T)pactor;
        pcloneptr += 1; // next word
        *pcloneptr = sizeof(glover_actor); // ACTOR_SIZE;
        pcloneptr += 1;

        // clone here
        gmemcpy((BYTE_T*)pactor, (BYTE_T*)pcloneptr, ACTOR_SIZE);

        // + F0
        pcloneptr += ACTOR_SIZE/4;

        // TODO identify and clone all pointers in actor
        // clone animation state
        // 0x60 bytes per actor if not NULL
        WORD_T *panimation = (WORD_T*)pactor->pproperties;
        pcloneptr = clone_additional(panimation, pcloneptr, sizeof(actor_properties));

        // clone collision ptr
        // 7C bytes
        WORD_T *pcollision = (WORD_T*)pactor->pcollision;
        pcloneptr = clone_additional(pcollision, pcloneptr, sizeof(actor_collision));


        pactor = pactor->pnext; // next value
    } while (pactor != (glover_actor*)ACTOR_HEAP_START);

    // clone switches and conditions
    get_ptr(switch_t, pswitch, SWITCH_LIST_START, 1);
    do {
        pcloneptr = clone_additional((WORD_T*)pswitch, pcloneptr, sizeof(switch_t));
        pcloneptr = clone_additional((WORD_T*)pswitch->ppuzzle1, pcloneptr, sizeof(puzzle_t));
        pcloneptr = clone_additional((WORD_T*)pswitch->ppuzzle2, pcloneptr, sizeof(puzzle_t));
        pcloneptr = clone_additional((WORD_T*)pswitch->pcond1, pcloneptr, sizeof(condition_t));
        pcloneptr = clone_additional((WORD_T*)pswitch->pcond2, pcloneptr, sizeof(condition_t));

        pswitch = pswitch->pnext;
    } while (pswitch != (switch_t*)SWITCH_LIST_START);


    get_ptr(obj_bank_t, pobj, OBJ_BANK, 1);
    pcloneptr = clone_other_objects(pcloneptr, pobj);

    // store obj bank if requested
    /*if (gpatch.clone_obj_bank) {
        clone_obj_bank(pcloneptr, slot);
    }*/

    // clone garibs TODO
    /*get_ptr(garib_t, pgaribs, GARIB_LIST_START, 1);
    do {
        pcloneptr = clone_additional((WORD_T*)pgaribs, pcloneptr, sizeof(garib_t));
        pgaribs = pgaribs->pnext;
    } while (pgaribs != (garib_t*)GARIB_LIST_START); */

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

    if (pcloneptr-pstart > ACTOR_HEAP_SIZE) {
        notify(&gpatch, "War: Sate oob!", MSG_TIME);
    } else {
        char *success_msg = "         bytes stored!";
        to_hexstr(pcloneptr-pstart, success_msg, sizeof(WORD_T));
        success_msg[8] = ' ';
        notify(&gpatch, success_msg, MSG_TIME);
    }
}

/**
 * This is maddening to write, but we really
 * do not want to save the WHOLE object bank because it is absolutely
 * unbeleiveably massive. Unless someone gives me another 4 MiB of RAM
 * I might just be eternally doomed to write terrible code.
 */
#define do_clone(obj, name) if (gstrncmp(obj->name, name, 0xC) == 0) { return TRUE; }
BOOLEAN clone_obj(obj_bank_t *pobj) {
    if (gstrncmp(pobj->name, "Plat_Plane", 0xC) == 0) {
        return TRUE;
    }else if (gstrncmp(pobj->name, "Act_Stats", 0xC) == 0) {
        return TRUE;
    } else if (gstrncmp(pobj->name, "Garib", 0x5) == 0) {
        return TRUE;
    } else if (gstrncmp(pobj->name, "UniqAct", 0xC) == 0) {
        return TRUE;
    } else if (gstrncmp(pobj->name, "Instruct", 0xC) == 0) {
        return TRUE;
    } else if (gstrncmp(pobj->name, "Uniq", 0x4) == 0) {
        return TRUE;
    }
    return FALSE;
}

WORD_T* clone_other_objects(WORD_T *pcloneptr, obj_bank_t *pobj) {
    do {
        if (clone_obj(pobj)) {
            pcloneptr = clone_additional((WORD_T*)pobj->pdata, pcloneptr, pobj->size);
        }

        pobj++; // next value
    } while (pobj->name[0] != '\0');

    return pcloneptr;
}

void clone_obj_bank(WORD_T *pcloneptr, u16 slot) {
    // load current map and store it
    get_ptr(u8, current_map, CURRENT_MAP, 1);
    lastmap[MAX_RESTORE_SLOTS+1] = *current_map;

    // actor value here is also the actor's next ptr
    // actor heap loops on itself
    // if we're back at the start we are done
    get_ptr(obj_bank_t, pobj, OBJ_BANK, 1);
    // current clone address

    if (!pcloneptr) {
        pcloneptr = ACTOR_HEAP_CLONE+ACTOR_HEAP_SIZE*slot;;
    }
    // get_ptr(WORD_T, pcloneptr, ACTOR_HEAP_CLONE+ACTOR_HEAP_SIZE*(MAX_RESTORE_SLOTS+1), 1);


    do {
        pcloneptr = clone_additional((WORD_T*)pobj->pdata, pcloneptr, pobj->size);

        pobj++; // next value
    } while (pobj->name[0] != '\0');
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

    notify(&gpatch, "Object Bank cloned!", MSG_TIME);
}

void restore_actors(WORD_T *pcloneptr, u16 slot) {
    // load current map and store it
    get_ptr(u8, current_map, CURRENT_MAP, 1);
    if (lastmap[slot] != *current_map && slot <= MAX_RESTORE_SLOTS) {
        gpatch.resume_restore = TRUE;
        gpatch.resume_timer = 0;
        // force load
        void (*load_map)(int) = LOAD_MAP;
        // void (*fade)() = FADE;
        void (*init_load)(int) = INIT_LOAD;
        //fade();
        init_load(1);
        load_map(lastmap[slot]);
        return;
    }
    gpatch.resume_restore = FALSE;

    WORD_T *pactor = NULL;
    if (!pcloneptr) {
        pcloneptr = ACTOR_HEAP_CLONE+ACTOR_HEAP_SIZE*slot;
    }
    // get_ptr(WORD_T, pcloneptr, ACTOR_HEAP_CLONE, 1);  // current clone address

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

    notify(&gpatch, "Actors restored!", MSG_TIME);
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

void toggle_show_objects() {
    get_ptr(glover_actor, pactor, GLOVER_ACTOR, 1);
    BOOLEAN was_shown = pactor->visible_flag;
    do {
        if (was_shown) {
            pactor->visible_flag = 0x00;
        } else {
            pactor->visible_flag = 0x64;
        }
        pactor = pactor->pnext;
    } while (pactor != (glover_actor*)GLOVER_ACTOR);
}


void trigger_al(menudef *pmenu) {
    get_ptr(WORD_T, al_inst, 0x801256FC, 1);
    if (*al_inst == 0x1040001D) {
        *al_inst = 0x1000001D; // branch always

        // this byte determines which mad to load
        // based on the current world (801E7533)
        // 0F == mainmenu
        // 13 == boss of world
        // etc
        get_ptr(BYTE_T, al_value, 0x801E7541, 1);
        *al_value = 0x13;
        get_ptr(BYTE_T, game_over, 0x801E7473, 1);
        *game_over = 1;
        pmenu->strings[2] = "Restore Game Over Code";
        get_ptr(WORD_T, lives, 0x80290190, 1);
        *lives = 0x00-1;
    } else {
        pmenu->strings[2] = "Trigger Afterlife";
        *al_inst = 0x1040001D; // regulat instruction
    }
}
