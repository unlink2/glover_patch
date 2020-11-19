#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "../../mbasic/src/utility.h"
#include "../../mbasic/src/mbasic.h"

extern int mb_msg_index;
extern char mb_msg[128];

void init_interpreter();

void free_code();
mb_error run_line(char *code);
mb_error mb_basic_update();

int gp_putch(int chr);

#endif
