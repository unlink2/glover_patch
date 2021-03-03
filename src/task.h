#ifndef __TASK_H__
#define __TASK_H__

#include "utility.h"

/**
 * Generic task function with void pointer
 */
typedef void (*task_call)(void*);

typedef struct task_t {
    task_call update;
    task_call render;
    BOOLEAN enable;
} task_t;

void init_task(task_t *task, task_call call, task_call render);

#endif
