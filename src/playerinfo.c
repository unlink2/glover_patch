#include "playerinfo.h"
#include "actor.h"

void update_playerinfo(playerinfo *ppi, memwatch *pmw) {
    if (!(ppi->flags & 0x80)) {
        return;
    }
    // set memwatch to display player info relevatn things
    clear_all_watch(pmw);

    get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);

    pmw->watch_addrs[0].lock = FALSE;
    pmw->watch_addrs[0].type = FLOAT_WATCH;
    pmw->watch_addrs[0].enabled = TRUE;
    pmw->watch_addrs[0].paddr = &pglover->xpos;
    gstrcpy(pmw->watch_addrs[0].name, "X");

    pmw->watch_addrs[1].lock = FALSE;
    pmw->watch_addrs[1].type = FLOAT_WATCH;
    pmw->watch_addrs[1].enabled = TRUE;
    pmw->watch_addrs[1].paddr = &pglover->ypos;
    gstrcpy(pmw->watch_addrs[1].name, "Y");

    pmw->watch_addrs[2].lock = FALSE;
    pmw->watch_addrs[2].type = FLOAT_WATCH;
    pmw->watch_addrs[2].enabled = TRUE;
    pmw->watch_addrs[2].paddr = &pglover->zpos;
    gstrcpy(pmw->watch_addrs[2].name, "Z");

    pmw->watch_addrs[3].lock = FALSE;
    pmw->watch_addrs[3].type = FLOAT_WATCH;
    pmw->watch_addrs[3].enabled = TRUE;
    pmw->watch_addrs[3].paddr = &pglover->angle;
    gstrcpy(pmw->watch_addrs[3].name, "A");

    pmw->watch_addrs[4].lock = FALSE;
    pmw->watch_addrs[4].type = FLOAT_WATCH;
    pmw->watch_addrs[4].enabled = TRUE;
    pmw->watch_addrs[4].paddr = &pglover->velx;
    gstrcpy(pmw->watch_addrs[4].name, "VX");

    pmw->watch_addrs[5].lock = FALSE;
    pmw->watch_addrs[5].type = FLOAT_WATCH;
    pmw->watch_addrs[5].enabled = TRUE;
    pmw->watch_addrs[5].paddr = &pglover->gravity;
    gstrcpy(pmw->watch_addrs[5].name, "VY");

    pmw->watch_addrs[6].lock = FALSE;
    pmw->watch_addrs[6].type = FLOAT_WATCH;
    pmw->watch_addrs[6].enabled = TRUE;
    pmw->watch_addrs[6].paddr = &pglover->velz;
    gstrcpy(pmw->watch_addrs[6].name, "VZ");

    get_ptr(glover_actor, pball, BALL_ACTOR, 1);

    pmw->watch_addrs[7].lock = FALSE;
    pmw->watch_addrs[7].type = FLOAT_WATCH;
    pmw->watch_addrs[7].enabled = TRUE;
    pmw->watch_addrs[7].paddr = &pball->velx;
    gstrcpy(pmw->watch_addrs[7].name, "BVX");

    pmw->watch_addrs[8].lock = FALSE;
    pmw->watch_addrs[8].type = FLOAT_WATCH;
    pmw->watch_addrs[8].enabled = TRUE;
    pmw->watch_addrs[8].paddr = &pball->gravity;
    gstrcpy(pmw->watch_addrs[8].name, "BVY");

    pmw->watch_addrs[9].lock = FALSE;
    pmw->watch_addrs[9].type = FLOAT_WATCH;
    pmw->watch_addrs[9].enabled = TRUE;
    pmw->watch_addrs[9].paddr = &pball->velz;
    gstrcpy(pmw->watch_addrs[9].name, "BVZ");
}

