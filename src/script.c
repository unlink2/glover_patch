#include "include/script.h"
#include "include/utility.h"
#include "include/typedefs.h"

// i do not know why but the compiler is refusing to see those delcarations
// even though they are clearly in utility.h
// and at this point im too tired to care
void to_floatstr(float, char *, WORD_T);
void to_hexstr_signed(WORD_T, char *, WORD_T);
void to_hexstr(WORD_T, char *, WORD_T);

char vm_code_buffer[VM_CODE_SIZE];
char out_buffer[VM_OUT_SIZE];

char *current_script = NULL;
void set_script(lispvm_t *pvm, char *script, char *out_buffer) {
    current_script = script;
    repl(pvm, current_script, out_buffer);
}

char* type_to_string(lisp_type type) {
    switch (type) {
        case TYPE_ATOM:
            return "[atom]";
        case TYPE_FUNCTION:
            return "[func]";
        case TYPE_NATIVE_FUNCTION:
            return "[native]";
        case TYPE_NUMBER:
            return "[number]";
        case TYPE_FLOAT:
            return "[float]";
        case TYPE_LIST:
            return "[list]";
        case TYPE_NULL:
            return "[null]";
        case TYPE_STRING:
            return "[string]";
        default:
            return "[nil]";
    }
}

void on_error(lispvm_t *pvm, lispeval_t *eval, char *buffer, char *out_buffer) {
    lmemset(out_buffer, 0, VM_OUT_SIZE);

    // copy string in an awful way
    // printf("%s in '%*.*s' %s %s\n", eval->pmessage, len, len, eval->value.ptoken, type_to_string(eval->value.type), buffer);
    // on error collapse stack
    lstrncpy(out_buffer, eval->pmessage, VM_OUT_SIZE);
    pvm->stack_ptr = 0;
}

short print_lisp_value(lispval_t *pval, char *out_buffer);

short print_list(lispval_t *pval, char *out_buffer) {
    out_buffer[0] = '(';
    out_buffer++;
    short len = 1;
    int i = 0;
    int total_len = pval->arg_len;
    while (pval->pnext && i < total_len) {
        pval = pval->pnext;
        if (pval) {
            short newlen = print_lisp_value(pval, out_buffer);
            out_buffer = out_buffer + newlen;
            if (pval->type == TYPE_LIST) {
                // use next for N iterations
                int arg_len = pval->arg_len;
                for (int i = 0; i < arg_len && pval->pnext; i++) {
                    pval = pval->pnext;
                }
            }
            if (pval->pnext && i+1 < total_len) {
                out_buffer[0] = ' ';
                out_buffer++;
                len++;
            }
        }
        i++;
    }
    out_buffer[0] = ')';
    out_buffer++;
    len++;
    return len;
}

// returns lenght of string added
short print_lisp_value(lispval_t *pval, char *out_buffer) {
    short len = 0;
    switch (pval->type) {
        case TYPE_LIST:
            len = print_list(pval, out_buffer);
            break;
        case TYPE_ATOM:
        case TYPE_FUNCTION:
            len = pval->token_len;
            lstrncpy(out_buffer, pval->ptoken, pval->token_len);
            break;
        case TYPE_STRING:
            len = pval->arg_len;
            lstrncpy(out_buffer, pval->value.pstring, pval->arg_len);
            break;
        case TYPE_NUMBER:
            to_hexstr(pval->value.intval, out_buffer, 4);
            len += lstrlen(out_buffer);
            break;
        case TYPE_FLOAT:
            to_floatstr(pval->value.floatval, out_buffer, 4);
            len += lstrlen(out_buffer);
            break;
        case TYPE_CHAR:
            len = 1;
            out_buffer[0] = (char)pval->value.intval;
            break;
        case TYPE_NIL:
            len = lstrlen("(nil)");
            lstrncpy(out_buffer, "(nil)", VM_OUT_SIZE);
            break;
        case TYPE_NULL:
            len = lstrlen("(null)");
            lstrncpy(out_buffer, "(null)", VM_OUT_SIZE);
            break;
        default:
            break;
    }

    return len;
}

char update_vm(lispvm_t *pvm, char *out_buffer) {
    // run a script if we have one
    // the onframe function is called
    if (current_script) {
        repl(pvm, "(onframe)", out_buffer);
        return 1;
    }

    return 0;
}

void repl(lispvm_t *pvm, char *pinput, char *out_buffer) {
    lispeval_t eval = eval_expression_from(pvm, pinput, lstrlen(pinput));

    if (eval.error != LISP_OK) {
        on_error(pvm, &eval, (char*)pinput, out_buffer);
    } else {
        print_lisp_value(&eval.value, out_buffer);
    }


    // on exit we reset
    if (pvm->exit) {
        reset_vm(pvm);
    }
}

void reset_vm(lispvm_t *pvm) {
    // init vm for scripting
    init_lisp_vm(pvm, vm_code_buffer, VM_CODE_SIZE);
    lmemset((unsigned char*)vm_code_buffer, 0, VM_CODE_SIZE);
    register_builtins(pvm);
    current_script = NULL;
}

lispvm_t vm;
