#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/utility.h"

static void test_get_next_token(void **state) {
    char *tokenstr = "Hello Thisisa  tokenstring \"Quoted string\""; // 3 tokens
    unsigned int size;

    char *t1 = get_next_token(tokenstr, &size);
    assert_true(strncmp(t1, "Hello", size) == 0);
    assert_int_equal(size, strlen("Hello"));

    char *t2 = get_next_token(t1+size, &size);
    assert_true(strncmp(t2, "Thisisa", size) == 0);
    assert_int_equal(size, strlen("Thisisa"));

    char *t3 = get_next_token(t2+size, &size);
    assert_true(strncmp(t3, "tokenstring", size) == 0);
    assert_int_equal(size, strlen("tokenstring"));


    char *t4 = get_next_token(t3+size, &size);
    assert_true(strncmp(t4, "\"Quoted string\"", size) == 0);
    assert_int_equal(size, strlen("\"Quoted string\""));

    // finally NULL
    char *t5 = get_next_token(t4+size, &size);
    assert_null(t5);
    assert_int_equal(size, 0);

    char *termstring = "Terminated<>Stringtest=test";
    char *t6 = get_next_token(termstring, &size);
    assert_true(strncmp(t6, "Terminated", size) == 0);
    assert_int_equal(size, strlen("Terminated"));

    char *t7 = get_next_token(t6+size, &size);
    assert_true(strncmp(t7, "<>", size) == 0);
    assert_int_equal(size, strlen("<>"));

    char *t8 = get_next_token(t7+size, &size);
    assert_true(strncmp(t8, "Stringtest", size) == 0);
    assert_int_equal(size, strlen("Stringtest"));

    char *t9 = get_next_token(t8+size, &size);
    assert_true(strncmp(t9, "=", size) == 0);
    assert_int_equal(size, strlen("="));

    char *t10 = get_next_token(t9+size, &size);
    assert_true(strncmp(t10, "test", size) == 0);
    assert_int_equal(size, strlen("test"));
}

int main() {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_get_next_token)
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
