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
#include "include/render.h"
#include "include/font8x8_basic.h"

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

static void test_decompress_font(void **state) {
    HWORD_T *pfont = my_malloc(0x2000*sizeof(HWORD_T));
    decompress_font((WORD_T*)font8x8_basic, pfont, 0x000F, 0xFFFF);

    // read from file
    HWORD_T *pfont_expected = my_malloc(0x2000*sizeof(HWORD_T));
    FILE *pfile = fopen("./tests/font_expected.dmp", "rb");
    fseek(pfile, 0, SEEK_END);
    long lsize = ftell(pfile);
    rewind(pfile);

    for (int i = 0; i < lsize; i += 2) {
        char f = fgetc(pfile);
        char s = fgetc(pfile);
#ifdef __ORDER_LITTLE_ENDIAN__
        // byte order swap -> expected file is in BE
        pfont_expected[i/2] = f << 8 | s;
#else
        pfont_expected[i/2] = s << 8 | f;
#endif
    }

    fclose(pfile);

    for (int i = 0; i < 0x2000; i++) {
        assert_int_equal(pfont[i], pfont_expected[i]); // assert not zero
    }

    my_free(pfont_expected);
    my_free(pfont);
}

static void test_gputs(void **state) {
    HWORD_T *pfont = my_malloc(0x2000*sizeof(HWORD_T));
    decompress_font((WORD_T*)font8x8_basic, pfont, 0xFFFF, 0x000F);
    HWORD_T *pframebuffer = my_malloc(SCREEN_W*SCREEN_H*sizeof(HWORD_T));
    HWORD_T *pframebuffer_expected = my_malloc(SCREEN_W*SCREEN_H*sizeof(HWORD_T));

    // test printing
    gputs("Hello, ", pframebuffer, 10, 10, (WORD_T*)font8x8_basic, 0x000F, 0xFFFF);
    gputsf("WORLD!", pframebuffer, 50, 60, pfont);

    FILE *fp = fopen("./tests/puts_expected.dmp", "rb");
    // this file is not BE!
    fread(pframebuffer_expected, SCREEN_W*SCREEN_H*sizeof(HWORD_T), 1, fp);
    fclose(fp);

    for (int i = 0; i < SCREEN_W*SCREEN_H; i++) {
        assert_int_equal(pframebuffer[i], pframebuffer_expected[i]);
    }

    my_free(pframebuffer_expected);
    my_free(pfont);
    my_free(pframebuffer);
}

static void test_memwatch_from_addr(void **state) {
    // address only matters on the n64 here we just malloc
    memwatch *pmw = memwatch_from_addr(0x00);
    assert_non_null(pmw);
    assert_non_null(pmw->pstr);
    my_free(pmw->pstr);
    my_free(pmw);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_gmemcpy),
        cmocka_unit_test(test_gmemset),
        cmocka_unit_test(test_gstrlen),
        cmocka_unit_test(test_memwatch_from_addr),
        cmocka_unit_test(test_decompress_font),
        cmocka_unit_test(test_gputs)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
