#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"

lb_interpreter *interpreter = NULL;

#define HEAP_SIZE 256000

char heap[HEAP_SIZE];

int lb_msg_index = 0;
char lb_msg[128];

lb_visitor_result lb_peek(lb_callable *callable, lb_interpreter *interpreter, lb_linked_list *arguments, lb_token *token) {
    lb_visitor_result result = lb_visitor_result_make(LB_VR_OBJECT);
    result.object = lb_object_make(LB_NUMBER);

    lb_object *a1 = arguments->generic;
    lb_object *a2 = lb_linked_list_get(arguments, 1)->generic;
    if (a1->type != LB_NUMBER || a2->type != LB_NUMBER) {
        return result;
    }

    lb_memcpy(&result.object.num, (void*)a1->num, a2->num);

    return result;
}

void init_interpreter() {
    if (interpreter) {
        return;
    }
    lb_malloc = lb_mem_malloc;
    lb_free = lb_mem_free;
    lb_fputc = gp_putch;

    lb_mem_heap_init(heap, HEAP_SIZE);
    lb_memset(lb_msg, 0x00, 128);
    lb_msg_index = 0;

    interpreter = lb_interpreter_init();
    lb_interpreter_register_native(interpreter, "peek", 2, lb_peek);

}

void on_scanner_error(lb_scanner *scanner, lb_error *error) {
    gp_putch('S', NULL);
    gp_putch('E', NULL);
}

void on_parser_error(lb_parser *parser, lb_error *object) {
    gp_putch('P', NULL);
    gp_putch('E', NULL);
}

void on_interpreter_error(lb_interpreter *interpreter, lb_error *object) {
    gp_putch('R', NULL);
    gp_putch('E', NULL);
}

void run(char *code) {
    lb_scanner *scanner = lb_scanner_init(code, "gp");
    lb_parser *parser = lb_parser_init(lb_scanner_scan_tokens(scanner));
    parser->on_parser_error = on_parser_error;

    lb_linked_list *stmts = lb_parser_parse(parser);

    if (parser->error.type != LB_NO_ERROR) {
        // TODO report error
        gp_putch('A', NULL);
    } else {
        interpreter->on_runtime_error = on_interpreter_error;
        lb_interpreter_interprete(interpreter, stmts);
    }

    lb_scanner_free(scanner);
    lb_stmt_list_free(stmts);
    lb_parser_free(parser);
}


int gp_putch(int chr, void *f) {
    lb_msg[lb_msg_index++] = chr;
    return chr;
}
