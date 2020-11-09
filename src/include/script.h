#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "../../mlisp/src/mlisp.h"

#define VM_OUT_SIZE 128
#define VM_CODE_SIZE 1024

extern char vm_code_buffer[VM_CODE_SIZE];
extern char out_buffer[VM_OUT_SIZE];
extern ml_lispvm_t vm;

void set_script(ml_lispvm_t *pvm, char *script, char *out_buffer);

void reset_vm(ml_lispvm_t *pvm);
void repl(ml_lispvm_t *pvm, char *pinput, char *out_buffer);


char update_vm(ml_lispvm_t *pvm, char *out_buffer);

#endif
