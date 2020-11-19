#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "../../lbasic/src/utility.h"
#include "../../lbasic/src/lbasic.h"

extern int lb_msg_index;
extern char lb_msg[128];

void init_interpreter();

lb_error run_line(char *code);
lb_error lb_basic_update();

int gp_putch(int chr);

#endif
