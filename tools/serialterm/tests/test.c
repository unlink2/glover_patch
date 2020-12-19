#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


int main() {
    const struct CMUnitTest tests[] = {
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
