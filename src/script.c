#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"

lb_interpreter *interpreter = NULL;

#define HEAP_SIZE 256000

char heap[HEAP_SIZE];

int lb_msg_index = 0;
char lb_msg[128];

void init_interpreter() {
    if (interpreter) {
        return;
    }
    lb_malloc = lb_mem_malloc;
    lb_free = lb_mem_free;
    lb_putchar = gp_putch;

    lb_mem_heap_init(heap, HEAP_SIZE);
    lb_memset(lb_msg, 0x00, 128);
    lb_msg_index = 0;

    interpreter = lb_interpreter_init();
}

void run(char *code) {
    lb_scanner *scanner = lb_scanner_init(code);
    lb_parser *parser = lb_parser_init(lb_scanner_scan_tokens(scanner));

    lb_linked_list *stmts = lb_parser_parse(parser);

    if (parser->error.type != LB_NO_ERROR) {
        // TODO report error
        return;
    }
    lb_interpreter_interprete(interpreter, stmts);

    lb_scanner_free(scanner);
    lb_stmt_list_free(stmts);
    lb_parser_free(parser);
}


int gp_putch(int chr) {
    lb_msg[lb_msg_index++] = chr;
    return chr;
}
