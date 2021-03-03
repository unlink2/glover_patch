#include "task.h"

void init_task(task_t *task, task_call call, task_call render) {
    task->update = call;
    task->render = render;
    task->enable = FALSE;
}

