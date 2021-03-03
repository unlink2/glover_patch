#ifndef __FRAME_TIMER_H__
#define __FRAME_TIMER_H__

#include "task.h"
#include "utility.h"

typedef struct frametimer_t {
    task_t task;
    u16 max;
    u16 ticks;
    BOOLEAN tick_state;
    HWORD_T *font;
} frametimer_t;

void init_frametimer(frametimer_t *frametimer);

void frametimer_update(void *ftptr);
void frametimer_render(void *ftptr);

#endif 
