#ifndef __TASK_H__
#define __TASK_H__

/**
 * Generic task function with void pointer
 */
typedef void (*task_call)(void*);

typedef struct task_t {
    task_call *run;
} task_t;

void init_task(task_t *task, task_call *call);

#endif
