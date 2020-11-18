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
#include "include/actor.h"
#include "include/savefile.h"
#include "include/matrix.h"

/**
 * This is a somewhat incomplete unit test file
 * due to the nature of the project we can only test
 * in a limited capacity
 */

static void test_gstrcpy(void **state) {
    char dest[24] = "Not same";
    char *src = "Hello World!";
    assert_string_not_equal(dest, src);
    gstrcpy(dest, src);
    assert_string_equal(dest, src);
}

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

static void test_gstrncmp(void **state) {
    assert_int_equal(gstrncmp("Hello", "Hello", 16), 0);
    assert_int_equal(gstrncmp("Hello,", "Hello", 16), 1);
    assert_int_equal(gstrncmp("Hello", "Hello++", 16), -2);
    assert_int_equal(gstrncmp("HeLLo", "Hello", 16), 2);
    assert_int_equal(gstrncmp("heLLO", "hello", 2), 0);
    assert_int_equal(gstrncmp("ObjBank.mp", "ObjBank.mp", 0xC), 0);
}

static void test_split_space(void **state) {
    char *str = my_malloc(16);
    strcpy(str, "8029030C 22");
    char *head;
    char *tail;
    split_space(str, &head, &tail);

    assert_string_equal(head, "8029030C");
    assert_non_null(tail);
    assert_string_equal(tail, "22");

    strcpy(str, "HelloWorld");
    split_space(str, &head, &tail);
    assert_string_equal(head, "HelloWorld");
    assert_null(tail);

    my_free(str);
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

    // test full transparent puts
    // should not overwrite H due to the last byte being 0 (transparent)
    draw_char('\0', pframebuffer, 10, 10, (WORD_T*)font8x8_basic, 0x000F, 0xFFF0);

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

static void test_to_hexstr(void **state) {
    char *pstr = my_malloc(20);

    to_hexstr(0x80106901, pstr, sizeof(unsigned int));
    assert_string_equal(pstr, "80106901");

    to_hexstr_signed(0x14, pstr, sizeof(unsigned char));
    assert_string_equal(pstr, "14");

    to_hexstr_signed(0xEC, pstr, sizeof(unsigned char));
    assert_string_equal(pstr, "-14");


    my_free(pstr);
}

// sizes are adjusted for amd64 and verified by hand before entered
// size may vary due to pointer size differences
static void test_struct_size(void **state) {
    assert_int_equal(sizeof(glover_actor), 0x120);
    assert_int_equal(sizeof(actor_collision), 0x84+0x114);
    assert_int_equal(sizeof(actor_properties), 0x60);
    assert_int_equal(sizeof(save_file), 0x1C);
    assert_int_equal(sizeof(collision_polygon), 0x78);
    assert_int_equal(sizeof(camera_t), CAMERA_ACTOR_SIZE);

    assert_int_equal(sizeof(condition_t), 0x30);
    assert_int_equal(sizeof(puzzle_t), 0x20);
    assert_int_equal(sizeof(switch_t), 0x50);
    assert_int_equal(sizeof(garib_t), 0xB0);
    assert_int_equal(sizeof(model_entry_t), 0x68);
    assert_int_equal(sizeof(model_data_list_t), 0x10);
    assert_int_equal(sizeof(act_shad_t), 0x40);
    assert_int_equal(sizeof(model_properties), 0x50);
    assert_int_equal(sizeof(obj_sprite_t), 0x50);
}

static void test_m3_mul_v3(void **state) {
    vector3 expected;
    expected.x = 15;
    expected.y = 7;
    expected.z = 23;

    vector3 v3;
    v3.x = 2;
    v3.y = 6;
    v3.z = 1;

    matrix3 m3;
    m3.x.x = 1;
    m3.x.y = 2;
    m3.x.z = 1;

    m3.y.x = 0;
    m3.y.y = 1;
    m3.y.z = 1;

    m3.z.x = 2;
    m3.z.y = 3;
    m3.z.z = 1;

    vector3 result;
    m3_mul_v3(&m3, &v3, &result);

    assert_int_equal((int)expected.x, (int)result.x);
    assert_int_equal((int)expected.y, (int)result.y);
    assert_int_equal((int)expected.z, (int)result.z);

    vector3 points[4];
    init_vector3(&points[0], 25, 25, 0);
    init_vector3(&points[1], 50, 50, 0);
    init_vector3(&points[2], 25, 50, 0);
    init_vector3(&points[3], 50, 25, 0);

    for (int i = 0; i < 4; i++) {
        m3_mul_v3(&projection, &points[i], &result);
        assert_int_equal(result.x, points[i].x);
        assert_int_equal(result.y, points[i].y);
        assert_int_equal(result.z, points[i].z);
    }
}

static void test_gpow(void **state) {
    assert_int_equal(gpow(5, 0), 1);
    assert_int_equal(gpow(5, 5), 5*5*5*5*5);
    assert_int_equal(gpow(6, 3), 6*6*6);
}

static void test_str_reverse(void **state) {
    char *str = my_malloc(6);
    strcpy(str, "olleH");
    str_reverse(str, 5);
    assert_string_equal(str, "Hello");
    my_free(str);
}

static void test_to_decstr(void **state) {
    char *str = my_malloc(6);
    to_decstr(123, str, 0);
    assert_string_equal(str, "123");
    to_decstr(456, str, 5);
    assert_string_equal(str, "00456");
    my_free(str);
}

static void test_to_floatstr(void **state) {
    char *str = my_malloc(64);
    to_floatstr_pad(3.1415f, str, 3, 0);
    assert_string_equal(str, "3.141");

    to_floatstr_pad(3.1415f, str, 0, 0);
    assert_string_equal(str, "3");

    to_floatstr_pad(-3.1415f, str, 3, 0);
    assert_string_equal(str, "-3.141");

    to_floatstr_pad((float)0xFF800000, str, 0, 0);
    assert_string_equal(str, "-I");

    to_floatstr_pad((float)0x7F800000, str, 0, 0);
    assert_string_equal(str, "I");

    my_free(str);
}

static void test_from_hexstr(void **state) {
    assert_int_equal(from_hexstr("AaC2", 4), 0xAAC2);
    assert_int_equal(from_hexstr("Aa", 2), 0xAA);
    assert_int_equal(from_hexstr("AaC2", 2), 0xAA);
}

static void test_is_arg(void **state) {
    assert_true(is_arg("-htest", "-h"));
    assert_true(is_arg("--h", "--h"));
    assert_false(is_arg("-h", "--h"));
    assert_false(is_arg("--otest", "-h"));
}

static void test_parse_arg(void **state) {
    arg a1 = parse_arg("testargument", "test");

    assert_string_equal(a1.key, "test");
    assert_string_equal(a1.value, "argument");

    arg a2 = parse_arg("--flag", "--flag");

    assert_string_equal(a2.key, "--flag");
    assert_int_equal(a2.value[0], '\0');

    arg a3 = parse_arg("--flag", "--o");

    assert_null(a3.key);
    assert_null(a3.value);

    arg a4 = parse_arg("-otest", "-o");
    assert_string_equal(a4.key, "-o");
    assert_string_equal(a4.value, "test");
}




int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_gstrcpy),
        cmocka_unit_test(test_gmemcpy),
        cmocka_unit_test(test_gmemset),
        cmocka_unit_test(test_gstrlen),
        cmocka_unit_test(test_gstrncmp),
        cmocka_unit_test(test_decompress_font),
        cmocka_unit_test(test_gputs),
        cmocka_unit_test(test_to_hexstr),
        cmocka_unit_test(test_struct_size),
        cmocka_unit_test(test_m3_mul_v3),
        cmocka_unit_test(test_gpow),
        cmocka_unit_test(test_str_reverse),
        cmocka_unit_test(test_to_decstr),
        cmocka_unit_test(test_to_floatstr),
        cmocka_unit_test(test_from_hexstr),
        cmocka_unit_test(test_is_arg),
        cmocka_unit_test(test_parse_arg),
        cmocka_unit_test(test_split_space)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
