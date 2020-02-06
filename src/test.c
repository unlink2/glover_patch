#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/memory.h"
#include "include/typedefs.h"
#include "include/utility.h"
#include "include/memwatch.h"

/**
 * This is a somewhat incomplete unit test file
 * due to the nature of the project we can only test
 * in a limited capacity
 */

static void test_gmemcpy(void **state) {
    BYTE_T *t1 = my_malloc(15);
    BYTE_T *t2 = my_malloc(15);
    for (int i = 0; i < 15; i++) {
        t1[i] = i;
    }
    gmemcpy(t1, t2, 15);

    for (int i = 0; i < 15; i++) {
        assert_int_equal(t1[i], t2[i]);
    }

    my_free(t1);
    my_free(t2);
}

static void test_gmemset(void **state) {
    BYTE_T *t = my_malloc(15);
    gmemset(t, 0xDD, 15);
    for (int i = 0; i < 15; i++) {
        assert_int_equal(t[i], 0xDD);
    }
    my_free(t);
}

static void test_gstrlen(void **state) {
    char str[] = "Hello World!";
    assert_int_equal(gstrlen(str), 12);
}

static void test_memwatch_from_addr(void **state) {
    // address only matters on the n64 here we just malloc
    memwatch *pmw = memwatch_from_addr(0x00);
    assert_non_null(pmw);
    assert_non_null(pmw->pstr);
    my_free(pmw);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_gmemcpy),
        cmocka_unit_test(test_gmemset),
        cmocka_unit_test(test_gstrlen),
        cmocka_unit_test(test_memwatch_from_addr)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
