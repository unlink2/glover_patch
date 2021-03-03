#include "frametimer.h"
#include "render.h"

void init_frametimer(frametimer_t *frametimer) {
    init_task(&frametimer->task, frametimer_update, frametimer_render);
    frametimer->max = 0;
    frametimer->ticks = 0;
    frametimer->tick_state = 0;
}

void frametimer_update(void *ftptr) {
    frametimer_t *ft = (frametimer_t*)ftptr;
    ft->task.enable = ft->max;

    if (!ft->task.enable) {
        return;
    }

    ft->ticks++;
    if (ft->ticks > ft->max) {
        ft->ticks = 0;
        ft->tick_state = !ft->tick_state;
    }

}

void frametimer_render(void *ftptr) {
    frametimer_t *ft = (frametimer_t*)ftptr;
    if (!ft->task.enable) {
        return;
    }
    const int x = 20;
    const int y = 20;

    if (ft->tick_state) {
        gputsrdp("1", x, y, ft->font);
    } else {
        gputsrdp("0", x, y, ft->font);
    }
}
