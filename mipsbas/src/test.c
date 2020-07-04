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
#include "include/lispvalue.h"

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

    char *termstring = "Terminated()Stringtest,test";
    char *t6 = get_next_token(termstring, &size);
    assert_true(strncmp(t6, "Terminated", size) == 0);
    assert_int_equal(size, strlen("Terminated"));

    char *t7 = get_next_token(t6+size, &size);
    assert_true(strncmp(t7, "()", size) == 0);
    assert_int_equal(size, strlen("()"));

    char *t8 = get_next_token(t7+size, &size);
    assert_true(strncmp(t8, "Stringtest", size) == 0);
    assert_int_equal(size, strlen("Stringtest"));

    char *t9 = get_next_token(t8+size, &size);
    assert_true(strncmp(t9, ",", size) == 0);
    assert_int_equal(size, strlen(","));

    char *t10 = get_next_token(t9+size, &size);
    assert_true(strncmp(t10, "test", size) == 0);
    assert_int_equal(size, strlen("test"));
}

static void test_init_lispval(void **state) {
    lispval_t lvt;
    init_lisp_value(&lvt);
    assert_null(lvt.name);
    assert_int_equal(lvt.type, TYPE_NULL);
}

static void test_lstrlen(void **state) {
    char str[] = "Hello World!";
    assert_int_equal(lstrlen(str), 12);
}

static void test_lstrncmp(void **state) {
    assert_int_equal(lstrncmp("Hello", "Hello", 16), 0);
    assert_int_equal(lstrncmp("Hello,", "Hello", 16), 1);
    assert_int_equal(lstrncmp("Hello", "Hello++", 16), -2);
    assert_int_equal(lstrncmp("HeLLo", "Hello", 16), 2);
    assert_int_equal(lstrncmp("heLLO", "hello", 2), 0);
}

static void test_lmemcpy(void **state) {
    char t1[15];
    char t2[15];
    for (int i = 0; i < 15; i++) {
        t1[i] = i;
    }
    lmemcpy((char*)t1, (char*)t2, 15);

    for (int i = 0; i < 15; i++) {
        assert_int_equal(t1[i], t2[i]);
    }

}

static void test_lmemset(void **state) {
    unsigned char t[16];
    lmemset((char*)t, 0xDD, 15);
    for (int i = 0; i < 15; i++) {
        assert_int_equal(t[i], 0xDD);
    }
}

static void test_lstrcpy(void **state) {
    char dest[24] = "Not same";
    char *src = "Hello World!";
    assert_string_not_equal(dest, src);
    lstrcpy(dest, src);
    assert_string_equal(dest, src);
}

static void test_init_lispvm(void **state) {
    lispvm_t vm;
    init_lisp_vm(&vm);
    assert_int_equal(vm.heap_ptr, 0);
    assert_int_equal(vm.stack_ptr, 0);
}


int main() {
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_get_next_token),
            cmocka_unit_test(test_lstrlen),
            cmocka_unit_test(test_lstrncmp),
            cmocka_unit_test(test_lmemcpy),
            cmocka_unit_test(test_lmemset),
            cmocka_unit_test(test_lstrcpy),

            cmocka_unit_test(test_init_lispval),
            cmocka_unit_test(test_init_lispvm)
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
