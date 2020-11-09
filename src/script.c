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
void set_script(ml_lispvm_t *pvm, char *script, char *out_buffer) {
    current_script = script;
    repl(pvm, current_script, out_buffer);
}

char* type_to_string(ml_lisp_type type) {
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

void on_error(ml_lispvm_t *pvm, ml_lispeval_t *eval, char *buffer, char *out_buffer) {
    ml_memset(out_buffer, 0, VM_OUT_SIZE);

    // copy string in an awful way
    // printf("%s in '%*.*s' %s %s\n", eval->pmessage, len, len, eval->value.ptoken, type_to_string(eval->value.type), buffer);
    // on error collapse stack
    ml_strncpy(out_buffer, eval->pmessage, VM_OUT_SIZE);
    pvm->stack_ptr = 0;
}

short print_lisp_value(ml_lispobject_t *pval, char *out_buffer);

short print_list(ml_lispobject_t *pval, char *out_buffer) {
    out_buffer[0] = '(';
    out_buffer++;
    pval = pval->value.plist;
    short len = 0;
    while (pval) {
        if (pval) {
            short newlen = print_lisp_value(pval, out_buffer);
            out_buffer = out_buffer + newlen;
            len += newlen;
            if (pval->pnext) {
                out_buffer[0] = ' ';
                out_buffer++;
                len++;
            } else {
                out_buffer[0] = ')';
                out_buffer++;
                len++;
            }
        }
        pval = pval->pnext;
    }
    out_buffer++;
    len++;
    return len;
}

// returns lenght of string added
short print_lisp_value(ml_lispobject_t *pval, char *out_buffer) {
    short len = 0;
    switch (pval->type) {
        case TYPE_LIST:
            len = print_list(pval, out_buffer);
            break;
        case TYPE_ATOM:
        case TYPE_FUNCTION:
            len = pval->token_len;
            ml_strncpy(out_buffer, pval->ptoken, pval->token_len);
            break;
        case TYPE_STRING:
            len = pval->arg_len;
            ml_strncpy(out_buffer, pval->value.pstring, pval->arg_len);
            break;
        case TYPE_NUMBER:
            to_hexstr(pval->value.intval, out_buffer, 4);
            len += ml_strlen(out_buffer);
            break;
        case TYPE_FLOAT:
            to_floatstr(pval->value.floatval, out_buffer, 4);
            len += ml_strlen(out_buffer);
            break;
        case TYPE_CHAR:
            len = 1;
            out_buffer[0] = (char)pval->value.intval;
            break;
        case TYPE_NIL:
            len = ml_strlen("(nil)");
            ml_strncpy(out_buffer, "(nil)", VM_OUT_SIZE);
            break;
        case TYPE_NULL:
            len = ml_strlen("(null)");
            ml_strncpy(out_buffer, "(null)", VM_OUT_SIZE);
            break;
        default:
            break;
    }

    return len;
}

char update_vm(ml_lispvm_t *pvm, char *out_buffer) {
    // run a script if we have one
    // the onframe function is called
    if (current_script) {
        repl(pvm, "(if '(isdef 'onframe) '(onframe))", out_buffer);
        return 1;
    }

    return 0;
}

void repl(ml_lispvm_t *pvm, char *pinput, char *out_buffer) {
    ml_lispeval_t eval = ml_eval_expression_from(pvm, pinput, ml_strlen(pinput));

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

void reset_vm(ml_lispvm_t *pvm) {
    // init vm for scripting
    ml_init_lisp_vm(pvm, vm_code_buffer, VM_CODE_SIZE);
    ml_memset((unsigned char*)vm_code_buffer, 0, VM_CODE_SIZE);
    ml_register_builtins(pvm);
    current_script = NULL;
}

ml_lispvm_t vm;
