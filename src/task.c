#include "task.h"

void init_task(task_t *task, task_call *call) {
    task->run = call;
}

