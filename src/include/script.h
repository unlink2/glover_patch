#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "../../libex/src/utility.h"
#include "../../libex/src/parser.h"
#include "../../libex/src/scanner.h"
#include "../../libex/src/interpreter.h"
#include "../../libex/src/basicmalloc.h"

extern int lb_msg_index;
extern char lb_msg[128];

void init_interpreter();

void run(char *code);

int gp_putch(int chr, void *f);

#endif
