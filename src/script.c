#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"
#include "../mbasic/src/basicmalloc.h"

mb_basic *basic = NULL;

#define HEAP_SIZE 256000

char heap[HEAP_SIZE];

void init_interpreter() {
    if (basic) {
        return;
    }
    mb_malloc = mb_mem_malloc;
    mb_free = mb_mem_free;

    mb_mem_heap_init(heap, HEAP_SIZE);

    basic = mb_basic_init(NULL);

    s8 is_no_line;
    mb_line *l1 = mb_line_init(NULL, "10 a = 1", &is_no_line);
    mb_basic_run_line(basic, l1);
}
