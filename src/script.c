#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"
#include "../lbasic/src/basicmalloc.h"

lb_basic *basic = NULL;

#define HEAP_SIZE 256000

char heap[HEAP_SIZE];

int lb_msg_index = 0;
char lb_msg[128];

void init_interpreter() {
    if (basic) {
        return;
    }
    lb_malloc = lb_mem_malloc;
    lb_free = lb_mem_free;
    lb_putchar = gp_putch;

    lb_mem_heap_init(heap, HEAP_SIZE);
    lb_memset(lb_msg, 0x00, 128);
    lb_msg_index = 0;

    basic = lb_basic_init(NULL);
    basic->yield = 1;
    basic->yield_in = 1;
}

lb_error run_line(char *code) {
    lb_error error;
    error.error = 0;
    s8 is_no_line;
    lb_line *line = lb_line_init(NULL, code, &is_no_line);
    if (is_no_line) {
        error = lb_basic_run_line(basic, line);
        lb_line_free(line);
    } else {
        s32 len = lb_strlen(code);
        char *cpy = lb_malloc(len+1);
        lb_strncpy(cpy, code, len);
        cpy[len] = '\0';
        line->code = cpy;
        line->free_code = 1;
        lb_basic_insert_line(basic, line);
    }

    return error;
}

lb_error lb_basic_update() {
    lb_error error;
    error.error = 0;
    if (basic->running) {
        error = lb_basic_run_prog(basic);
    }

    if (error.error) {
        lb_msg_index = 1;
        lb_strncpy(lb_msg, error.message, lb_strlen(error.message));
    }

    return error;
}

int gp_putch(int chr) {
    lb_msg[lb_msg_index++] = chr;
    return chr;
}
