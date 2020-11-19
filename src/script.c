#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"
#include "../mbasic/src/basicmalloc.h"

mb_basic *basic = NULL;

#define HEAP_SIZE 256000

char heap[HEAP_SIZE];

int mb_msg_index = 0;
char mb_msg[128];

void init_interpreter() {
    if (basic) {
        return;
    }
    mb_malloc = mb_mem_malloc;
    mb_free = mb_mem_free;
    mb_putchar = gp_putch;

    mb_mem_heap_init(heap, HEAP_SIZE);
    mb_memset(mb_msg, 0x00, 128);
    mb_msg_index = 0;

    basic = mb_basic_init(NULL);
}

void free_code() {
    mb_linked_list *loop = basic->code;
    while (loop) {
        mb_line *l = loop->generic;
        mb_free(l->code);
        loop = loop->next;
    }
}

mb_error run_line(char *code) {
    mb_error error;
    error.error = 0;
    s8 is_no_line;
    mb_line *line = mb_line_init(NULL, code, &is_no_line);
    if (is_no_line) {
        error = mb_basic_run_line(basic, line);
        mb_line_free(line);
    } else {
        s32 len = mb_strlen(code);
        char *cpy = mb_malloc(len+1);
        mb_strncpy(cpy, code, len);
        cpy[len] = '\0';
        line->code = cpy;
        mb_basic_insert_line(basic, line);
    }

    return error;
}

mb_error mb_basic_update() {
    mb_error error;
    error.error = 0;
    if (basic->running) {
        error = mb_basic_run_prog(basic);
    }

    if (error.error) {
        mb_msg_index = 1;
        mb_strncpy(mb_msg, error.message, mb_strlen(error.message));
    }

    return error;
}

int gp_putch(int chr) {
    mb_msg[mb_msg_index++] = chr;
    return chr;
}
