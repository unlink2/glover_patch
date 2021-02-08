#include "othermenu.h"
#include "render.h"
#include "menu.h"
#include "utility.h"
#include "font8x8_basic.h"
#include "inputs.h"
#include "logic.h"
#include "debug.h"
#include "utility.h"
#include "actor.h"
#include "mainmenu.h"
#include "menu.h"

#define CHEATS_LEN 26
char *cheat_names[CHEATS_LEN];
u8 cheat_num = 0;
u8 map_id = 0;

enum OTHER_MENU_ENTRIES {
    ENTRY_INF_LIVES,
    ENTRY_INF_HEALTH,
    ENTRY_TRIGGER_AFTERLIFE,
    ENTRY_LOCK_POS,
    ENTRY_SUMMON_BALL,
    ENTRY_INF_JUMP,
    ENTRY_SPACE_CSS,
    ENTRY_CHEAT,
    ENTRY_LOCK_RNG,
    ENTRY_LOAD_MAP,
    ENTRY_DEBUG_GRAPH,
    ENTRY_DISABLE_PAUSE,
    ENTRY_AUTO_TIMER,
    ENTRY_IGT,
    ENTRY_PLAYER,
    ENTRY_STORE_BALL,
    ENTRY_GRAVITY,
    ENTRY_STICKY
};

void init_glover_menu(menudef *pmenu) {
    pmenu->size = 18;
    pmenu->cursor = 0;
    pmenu->strings[ENTRY_INF_LIVES] = "[ ] Infinite Lives";
    pmenu->strings[ENTRY_INF_HEALTH] = "[ ] Infinite Health";
    pmenu->strings[ENTRY_TRIGGER_AFTERLIFE] = "Trigger Afterlife";
    pmenu->strings[ENTRY_LOCK_POS] = "Lock Position";
    pmenu->strings[ENTRY_SUMMON_BALL] = "Summon Ball";
    pmenu->strings[ENTRY_INF_JUMP] = "Toggle Infinite Jump";
    pmenu->strings[ENTRY_SPACE_CSS] = "Toggle Space CS Skip";
    pmenu->strings[ENTRY_CHEAT] = "Cheat                  ";
    pmenu->strings[ENTRY_LOCK_RNG] = "Lock RNG (may crash)";
    pmenu->strings[ENTRY_LOAD_MAP] = "Load Map         ";
    pmenu->strings[ENTRY_DEBUG_GRAPH] = "Debug Graph";
    pmenu->strings[ENTRY_DISABLE_PAUSE] = "Disable Pause Menu";
    pmenu->strings[ENTRY_AUTO_TIMER] = "Autostart timer";
    pmenu->strings[ENTRY_IGT] = "Use IGT";
    pmenu->strings[ENTRY_PLAYER] = "Player Info";
    pmenu->strings[ENTRY_STORE_BALL] = "Store Ball";
    pmenu->strings[ENTRY_GRAVITY] = "[ ] Gravity";
    pmenu->strings[ENTRY_STICKY] = "Toggle Sticky";

    // set bool values
    put_bool(pmenu->pgpatch->infinite_lives, pmenu->strings[0]);
    put_bool(pmenu->pgpatch->infinite_hp, pmenu->strings[1]);
    get_ptr(BYTE_T, enable_gravity, ENABLE_GRAVITY, 1);
    put_bool(*enable_gravity, pmenu->strings[16]);

    pmenu->type[ENTRY_INF_LIVES] = MENU_BUTTON;
    pmenu->type[ENTRY_INF_HEALTH] = MENU_BUTTON;
    pmenu->type[ENTRY_TRIGGER_AFTERLIFE] = MENU_BUTTON;
    pmenu->type[ENTRY_LOCK_POS] = MENU_BUTTON;
    pmenu->type[ENTRY_INF_JUMP] = MENU_BUTTON;
    pmenu->type[ENTRY_SPACE_CSS] = MENU_BUTTON;
    pmenu->type[ENTRY_CHEAT] = MENU_VALUE_BYTE;
    pmenu->type[ENTRY_LOCK_RNG] = MENU_BUTTON;
    pmenu->type[ENTRY_LOAD_MAP] = MENU_VALUE_BYTE;
    pmenu->type[ENTRY_DEBUG_GRAPH] = MENU_BUTTON;
    pmenu->type[ENTRY_DISABLE_PAUSE] = MENU_BUTTON;
    pmenu->type[ENTRY_AUTO_TIMER] = MENU_BUTTON;
    pmenu->type[ENTRY_IGT] = MENU_BUTTON;
    pmenu->type[ENTRY_PLAYER] = MENU_BUTTON;
    pmenu->type[ENTRY_STORE_BALL] = MENU_BUTTON;
    pmenu->type[ENTRY_GRAVITY] = MENU_BUTTON;
    pmenu->type[ENTRY_STICKY] = MENU_BUTTON;
    pmenu->type[ENTRY_SUMMON_BALL] = MENU_BUTTON;

    pmenu->pvalue[7] = &cheat_num;
    pmenu->pvalue[9] = &map_id;

    pmenu->pactions = &glover_menu_select;
    pmenu->pupdate = &glover_menu_update;

    // init cheat names
    cheat_names[0] = "Locate Garibs";
    cheat_names[1] = "Call Ball";
    cheat_names[2] = "Checkpoints";
    cheat_names[3] = "Infinite Lives";
    cheat_names[4] = "Powerball";
    cheat_names[5] = "Control Ball";
    cheat_names[6] = "Infinite Energy";
    cheat_names[7] = "Enemy Ball";
    cheat_names[8] = "Low Gravity";
    cheat_names[9] = "Big Ball";
    cheat_names[10] = "Fish Eye";
    cheat_names[11] = "Rotate R";
    cheat_names[12] = "Rotate L";
    cheat_names[13] = "Mad";
    cheat_names[14] = "Death";
    cheat_names[15] = "Frog";
    cheat_names[16] = "Hercules";
    cheat_names[17] = "Speedup";
    cheat_names[18] = "Boomerang Ball";
    cheat_names[19] = "Vanishing Ball";
    cheat_names[20] = "Froggy";
    cheat_names[21] = "Secret";
    cheat_names[22] = "All Cheats";
    cheat_names[23] = "Level Select";
    cheat_names[24] = "Open Portals";
    cheat_names[25] = "Open Levels";

}

void glover_menu_select(menudef *pmenu) {
    switch(pmenu->cursor) {
        case ENTRY_INF_LIVES:
            pmenu->pgpatch->infinite_lives = !pmenu->pgpatch->infinite_lives;
            put_bool(pmenu->pgpatch->infinite_lives, pmenu->strings[0]);
            break;
        case ENTRY_INF_HEALTH:
            pmenu->pgpatch->infinite_hp = !pmenu->pgpatch->infinite_hp;
            put_bool(pmenu->pgpatch->infinite_hp, pmenu->strings[1]);
            break;
        case ENTRY_TRIGGER_AFTERLIFE:
            trigger_al(pmenu);
            break;
        case ENTRY_LOCK_POS:
            pmenu->pgpatch->lock_pos = !pmenu->pgpatch->lock_pos;
            store_glover_pos();
            break;
        case ENTRY_SUMMON_BALL:
            store_glover_pos();
            restore_glover_pos();
            break;
        case ENTRY_INF_JUMP:
            pmenu->pgpatch->infinite_jump = !pmenu->pgpatch->infinite_jump;
            break;
        case ENTRY_SPACE_CSS:
            pmenu->pgpatch->cutscene_skip = !pmenu->pgpatch->cutscene_skip;
            break;
        case ENTRY_CHEAT: {
            void (*trigger_cheat)(int, int, int) = TRIGGER_CHEAT;
            trigger_cheat((int)(*(u8*)(pmenu->pvalue[7])), 1, 8); // TODO what do other inputs do?
            break; }
        case ENTRY_LOCK_RNG: {
            pmenu->pgpatch->lockrng = !pmenu->pgpatch->lockrng;

            // start of rng function
            get_ptr(u32, rngval, RNG_FUNC, 1);
            if (pmenu->pgpatch->lockrng) {
                pmenu->strings[8] = "Unlock RNG";
                rngval[0] = 0x03E00008; // jr ra
                rngval[1] = 0x00000000; // nop
                //rngval[0] = 0x3C04801E; // lui a0, 801E
                //rngval[1] = 0x3484D3F0; // ori a0, D3F0
                //rngval[2] = 0x8C820000; // lw v0, 00(a0)
                //rngval[3] = 0x03E00008; // jr ra
                //rngval[4] = 0x00000000; // nop
            } else {
                pmenu->strings[8] = "Lock RNG";
                rngval[0] = 0x14800003; // original code
                rngval[1] = 0x27BDFFF8;
                //rngval[2] = 0x08051C1C;
                //rngval[3] = 0x00001021;
                //rngval[4] = 0x3C03801F;
            }
            break; }
        case ENTRY_LOAD_MAP: {
            void (*load_map)(int) = LOAD_MAP;
            // void (*fade)() = FADE;
            void (*init_load)(int) = INIT_LOAD;
            //fade();
            init_load(1);
            load_map((int)(*(u8*)(pmenu->pvalue[9])));
            break; }
        case ENTRY_DEBUG_GRAPH: {
            get_ptr(u8, debug_graph, DEBUG_GRAPH, 1);
            *debug_graph = !*debug_graph;
            break; }
        case ENTRY_DISABLE_PAUSE: {
            pmenu->pgpatch->disable_pause = !pmenu->pgpatch->disable_pause;
            // do it here instead to not cause issues in the title screen
            get_ptr(BYTE_T, nopause, DISABLE_PAUSE_FLAG, 1);
            *nopause = 0x00;
            break; }
        case ENTRY_AUTO_TIMER:
            pmenu->pgpatch->auto_timer = !pmenu->pgpatch->auto_timer;
            if (pmenu->pgpatch->auto_timer) {
                pmenu->strings[12] = "Disable Auto TImer";
            } else {
                pmenu->strings[12] = "Auto Timer";
            }
            break;
        case ENTRY_IGT:
            pmenu->pgpatch->use_igt = !pmenu->pgpatch->use_igt;
            if (pmenu->pgpatch->use_igt) {
                pmenu->strings[13] = "Use Custom Timer";
            } else {
                pmenu->strings[13] = "Use IGT";
            }
            break;
        case ENTRY_PLAYER:
            pmenu->pgpatch->pi.flags ^= 0x80;
            clear_all_watch(pmenu->pmemwatch);
            update_playerinfo(&pmenu->pgpatch->pi, pmenu->pmemwatch);
            break;
        case ENTRY_STORE_BALL: {
            get_ptr(WORD_T, store_ball, BALL_STORAGE, 1);
            *store_ball = 0x2A;
            break; }
        case ENTRY_GRAVITY: {
            get_ptr(BYTE_T, enable_gravity, ENABLE_GRAVITY, 1);
            *enable_gravity = !*enable_gravity;
            put_bool(*enable_gravity, pmenu->strings[16]);
            break; }
        case ENTRY_STICKY: {
           get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);
           pglover->actor_flags ^= 0x0000C000;
           break; }
        default:
            init_default_menu(pmenu);
            break;
    }
}

void glover_menu_update(menudef *pmenu) {
    /*if (pmenu->pgpatch->infinite_hp) {
        pmenu->strings[0] = "Disable Infinite Health";
    } else {
        pmenu->strings[0] = "Enable Infinite Health";
    }
    if (pmenu->pgpatch->infinite_lives) {
        pmenu->strings[1] = "Disable Infinite Lives";
    } else {
        pmenu->strings[1] = "Enable Infinite Lives";
    }*/
    if (pmenu->pgpatch->disable_pause) {
        pmenu->strings[ENTRY_DISABLE_PAUSE] = "Enable Pause Menu";
    } else {
        pmenu->strings[ENTRY_DISABLE_PAUSE] = "Disable Pause Menu";
    }
    // cheat select menu

    BYTE_T *cheatnum = (BYTE_T*)pmenu->pvalue[ENTRY_CHEAT];
    // if it has a name add the name otherwise just add the number
    if (*cheatnum < CHEATS_LEN) {
        // to_hexstr((WORD_T)cheat_names, pmenu->strings[7]+gstrlen("Cheat "), 4);
        gstrcpy(pmenu->strings[ENTRY_CHEAT]+gstrlen("Cheat "), cheat_names[*cheatnum]);
    } else {
        to_hexstr((WORD_T)*cheatnum, pmenu->strings[ENTRY_CHEAT]+gstrlen("Cheat "), 1);
    }

    // map id to string
    to_hexstr((WORD_T)map_id, pmenu->strings[ENTRY_LOAD_MAP]+gstrlen("Load Map "), 1);
}
