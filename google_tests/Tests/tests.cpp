// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include <iostream>

#ifndef FILE_DIR
#define FILE_DIR "../google_tests/test_files"
#endif

#pragma GCC diagnostic ignored "-Wformat-zero-length"


extern "C" {
#include "c_string_lib.h"
#include <string.h>
#include <stdint.h>
}

static size_t test_c_str_len(const char *string) {
    if (string == NULL || !string)
        return (size_t) NULL_PTR_ERR;
    size_t result = 0;
    while (string[result])
        result++;
    return result;
}

namespace {
    class ClassDeclaration : public testing::Test {
    protected:
        ClassDeclaration() = default;

        my_str_t string1{};
        my_str_t string2{};
        my_str_t string3{};


        void SetUp() override {
            my_str_create(&string1, 20);
            my_str_create(&string2, 20);
            my_str_create(&string3, 0);
        };

        void TearDown() override {
            my_str_free(&string1);
            my_str_free(&string2);
        }
    };

}

TEST_F(ClassDeclaration, my_str_create) {
    // test string parameters after create
    ASSERT_EQ(string1.size_m, 0);
    ASSERT_EQ(string1.capacity_m, 20);
    ASSERT_TRUE(string1.data != NULL);

    // test create return code if string != NULL
    my_str_free(&string1);
    ASSERT_EQ(my_str_create(&string1, 20), 0);

    // str is NULL
    my_str_free(&string1);
    ASSERT_EQ(my_str_create(NULL, 10), NULL_PTR_ERR);

    // max size
    size_t s = SIZE_MAX;
    int errcode = my_str_create(&string1, s);
    int errcode2 = my_str_create(&string1, s - 1);
    ASSERT_EQ((errcode == MEMORY_ALLOCATION_ERR) || (errcode2 == MEMORY_ALLOCATION_ERR), true);
}

TEST_F(ClassDeclaration, my_str_free) {
    string1.size_m = 3;

    // Check string parameters after free
    my_str_free(&string1);
    ASSERT_EQ(string1.size_m, 0);
    ASSERT_EQ(string1.capacity_m, 0);
    ASSERT_TRUE(string1.data == NULL);

}

TEST_F(ClassDeclaration, my_str_size) {
    string1.size_m = 12;

    // Check size of normal string
    ASSERT_EQ(string1.size_m, my_str_size(&string1));
    ASSERT_EQ(12, my_str_size(&string1));

    // Check size of NULL pointer (its zero)
    ASSERT_EQ(0, my_str_size(NULL));

}

TEST_F(ClassDeclaration, my_str_capacity) {
    my_str_free(&string1);
    my_str_create(&string1, 99);
    // Check capacity of normal string
    ASSERT_EQ(99, my_str_capacity(&string1));

    // check capacity of NULL pointer
    ASSERT_EQ(0, my_str_capacity(NULL));

}

TEST_F(ClassDeclaration, my_str_empty) {
    // Check empty string
    ASSERT_TRUE(my_str_empty(&string1));

    // Check NULL pointer
    ASSERT_TRUE(my_str_empty(NULL));

    // Check not empty string
    my_str_from_cstr(&string1, "hello", 20);
    ASSERT_FALSE(my_str_empty(&string1));


}

TEST_F(ClassDeclaration, my_str_from_cstr) {
    // create from adequate c-string
    int from_code = my_str_from_cstr(&string1, "hello, world!", 20);
    ASSERT_EQ(from_code, 0);

    // create from empty c-string
    from_code = my_str_from_cstr(&string1, "", 20);
    ASSERT_TRUE(my_str_empty(&string1));
    ASSERT_EQ(from_code, 0);

    //create with too small buffer
    from_code = my_str_from_cstr(&string1, "ggg", 2);
    ASSERT_EQ(from_code, BUFF_SIZE_ERR);

    //create with NULL pointer
    from_code = my_str_from_cstr(NULL, "hello", 20);
    ASSERT_EQ(from_code, NULL_PTR_ERR);

    // create from NULL
    from_code = my_str_from_cstr(&string1, NULL, 20);
    ASSERT_EQ(from_code, NULL_PTR_ERR);

    // create with buffsize 0
    my_str_free(&string1);
    my_str_create(&string1, 1);
    from_code = my_str_from_cstr(&string1, "hello", 0);
    ASSERT_EQ(from_code, 0);
    ASSERT_EQ(string1.capacity_m, test_c_str_len("hello"));

    // check if buffer size is OK
    my_str_free(&string1);
    my_str_create(&string1, 1);
    my_str_from_cstr(&string1, "hello", 20);
    ASSERT_EQ(string1.capacity_m, 20);

    // check if capacity will increase if needed
    my_str_free(&string1);
    my_str_create(&string1, 10);
    my_str_from_cstr(&string1, "hello, world!", 20);
    ASSERT_EQ(string1.capacity_m, 20);

    // check if capaciy won't change if it's not needed
    my_str_free(&string1);
    my_str_create(&string1, 19);
    my_str_from_cstr(&string1, "hello, world!", 20);
    ASSERT_EQ(string1.capacity_m, 19);

}

TEST_F(ClassDeclaration, my_str_getc) {
    my_str_from_cstr(&string1, "hello, world!", 20);

    // check inside position
    ASSERT_EQ(my_str_getc(&string1, 4), 'o');//stupid pvs says that this is "dangerous magic number" OMG
    ASSERT_EQ(my_str_getc(&string1, 10), 'l');

    // check 0 position
    ASSERT_EQ(my_str_getc(&string1, 0), 'h');

    // Check last position
    ASSERT_EQ(my_str_getc(&string1, 12), '!');

    // Bad index
    ASSERT_EQ(my_str_getc(&string1, -12), RANGE_ERR);
    ASSERT_EQ(my_str_getc(&string1, -20), RANGE_ERR);

    // NULL pointer
    ASSERT_EQ(my_str_getc(NULL, 12), NULL_PTR_ERR);

    // empty string
    my_str_free(&string1);
    my_str_create(&string1, 20);
    my_str_from_cstr(&string1, "", 20);
    ASSERT_EQ(my_str_getc(&string1, 0), RANGE_ERR);

}

TEST_F(ClassDeclaration, my_str_get_cstr) {
    my_str_from_cstr(&string1, "hello, world!", 20);

    // normal string
    ASSERT_STREQ("hello, world!", my_str_get_cstr(&string1));

    // empty string
    my_str_from_cstr(&string1, "", 20);
    ASSERT_STREQ("", my_str_get_cstr(&string1));

    // parse NULL pointer
    ASSERT_EQ(NULL, my_str_get_cstr(nullptr));

}

TEST_F(ClassDeclaration, my_str_putc) {
    my_str_from_cstr(&string1, "hello, world!", 20);

    // put inside the string
    my_str_putc(&string1, 1, 'a');
    ASSERT_STREQ(my_str_get_cstr(&string1), "hallo, world!");
    my_str_putc(&string1, 10, 'r');
    ASSERT_STREQ(my_str_get_cstr(&string1), "hallo, worrd!");

    //put on the borders
    my_str_putc(&string1, 0, 'g');
    ASSERT_STREQ(my_str_get_cstr(&string1), "gallo, worrd!");
    my_str_putc(&string1, 12, '?');
    ASSERT_STREQ(my_str_get_cstr(&string1), "gallo, worrd?");

    // put out of the borders
    ASSERT_EQ(my_str_putc(&string1, -1, 'a'), RANGE_ERR);
    ASSERT_EQ(my_str_putc(&string1, 13, 'a'), RANGE_ERR);
    ASSERT_STREQ(my_str_get_cstr(&string1), "gallo, worrd?");

    // put on the beginning of empty string
    my_str_free(&string1);
    my_str_create(&string1, 20);
    my_str_from_cstr(&string1, "", 20);
    ASSERT_EQ(my_str_putc(&string1, 0, 'a'), RANGE_ERR);

    // put in the NULL pointer
    ASSERT_EQ(my_str_putc(NULL, 0, '1'), NULL_PTR_ERR);

}

TEST_F(ClassDeclaration, my_str_append_c) {
    my_str_from_cstr(&string1, "Hello, world", 20);

    // Normal pushback
    my_str_append_c(&string1, '!');
    ASSERT_STREQ(my_str_get_cstr(&string1), "Hello, world!");

    // Check if push increases buffer if needed
    for (int i = 0; i < 10; i++)
        my_str_append_c(&string1, '!');
    ASSERT_EQ(my_str_capacity(&string1), 40);

    // Check normal output of function
    ASSERT_EQ(my_str_append_c(&string1, '!'), 0);

    // Check output if string of char is NULL
    ASSERT_EQ(my_str_append_c(&string1, '\0'), NULL_PTR_ERR);
    ASSERT_EQ(my_str_append_c(NULL, 'c'), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_popback) {
    my_str_from_cstr(&string1, "hello, world!", 20);

    // normal popback
    int last = my_str_popback(&string1);
    ASSERT_EQ((char) last, '!');
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // many popbacks
    for (int i = 0; i < 11; i++)
        last = my_str_popback(&string1);
    ASSERT_EQ((char) last, 'e');
    ASSERT_STREQ(my_str_get_cstr(&string1), "h");

    // pop from empty string
    my_str_popback(&string1);
    last = my_str_popback(&string1);
    ASSERT_EQ(last, RANGE_ERR);

    // pass NULL
    last = my_str_popback(NULL);
    ASSERT_EQ(last, NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_copy) {
    my_str_from_cstr(&string1, "hello", 20);

    //normal copy, reserve = 1
    int copy_code = my_str_copy(&string1, &string2, 1);
    ASSERT_EQ(copy_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello");
    ASSERT_EQ(my_str_capacity(&string2), my_str_capacity(&string2));

    // normal copy, reserve = 0
    my_str_copy(&string1, &string2, 0);
    ASSERT_EQ(copy_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello");
    ASSERT_EQ(my_str_capacity(&string2), my_str_capacity(&string1));

    // buffer too small, reserve = 0
    //    my_str_resize(&string2, 1, 'a');
    my_str_from_cstr(&string2, "a", 0);
    my_str_shrink_to_fit(&string2);
    my_str_copy(&string1, &string2, 0);
    ASSERT_EQ(copy_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello");
    ASSERT_EQ(my_str_capacity(&string2), my_str_size(&string1));

    // buffer too small, reserve = 1
    //    my_str_resize(&string2, 1, 'a');
    my_str_from_cstr(&string2, "a", 0);
    my_str_shrink_to_fit(&string2);
    my_str_copy(&string1, &string2, 1);
    ASSERT_EQ(copy_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello");
    ASSERT_EQ(my_str_capacity(&string2), my_str_capacity(&string1));

    // one of strings is NULL
    copy_code = my_str_copy(&string1, NULL, 1);
    ASSERT_EQ(copy_code, NULL_PTR_ERR);
    copy_code = my_str_copy(NULL, &string1, 1);
    ASSERT_EQ(copy_code, NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_clear) {
    my_str_from_cstr(&string1, "hello, world", 20);

    //normal clear
    my_str_clear(&string1);
    ASSERT_TRUE(my_str_empty(&string1));
    ASSERT_STREQ(my_str_get_cstr(&string1), "");

    // clearing NULL doesn't raise error
    my_str_clear(NULL);

    // clear empty string
    my_str_clear(&string1);
    ASSERT_TRUE(my_str_empty(&string1));
}

TEST_F(ClassDeclaration, my_str_insert_c) {
    my_str_from_cstr(&string1, "hello, world!", 20);

    //normal insert inside
    int in_code = my_str_insert_c(&string1, 'l', 2);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "helllo, world!");
    ASSERT_EQ(my_str_size(&string1), 14);

    // insert on the last position + 1
    in_code = my_str_insert_c(&string1, '!', 14);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "helllo, world!!");
    ASSERT_EQ(my_str_size(&string1), 15);

    // insert on the 0 position
    in_code = my_str_insert_c(&string1, 'h', 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hhelllo, world!!");
    ASSERT_EQ(my_str_size(&string1), 16);

    // bad position
    in_code = my_str_insert_c(&string1, '!', 20);
    ASSERT_EQ(in_code, RANGE_ERR);
    ASSERT_EQ(my_str_size(&string1), 16);

    // bad position
    in_code = my_str_insert_c(&string1, '!', -1);
    ASSERT_EQ(in_code, RANGE_ERR);
    ASSERT_EQ(my_str_size(&string1), 16);

    // insert in the empty string
    my_str_clear(&string1);
    in_code = my_str_insert_c(&string1, '!', 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "!");
    ASSERT_EQ(my_str_size(&string1), 1);

    // insert in NULL
    in_code = my_str_insert_c(NULL, '!', 0);
    ASSERT_EQ(in_code, NULL_PTR_ERR);

    // insert in empty string
    in_code = my_str_insert_c(&string3, 'h', 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_GE(string3.capacity_m, 2);
}

TEST_F(ClassDeclaration, my_str_insert) {
    my_str_from_cstr(&string1, "herld", 20);
    my_str_from_cstr(&string2, "llo, wo", 20);

    // normal insert
    int in_code = my_str_insert(&string1, &string2, 2);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_EQ(my_str_size(&string2), 7);

    //insert on the end of string
    my_str_from_cstr(&string1, "hello, ", 20);
    my_str_from_cstr(&string2, "world", 20);
    in_code = my_str_insert(&string1, &string2, 7);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_EQ(my_str_size(&string2), 5);

    // insert at the end of string
    my_str_from_cstr(&string1, "world", 20);
    my_str_from_cstr(&string2, "hello, ", 20);
    in_code = my_str_insert(&string1, &string2, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_EQ(my_str_size(&string2), 7);

    // insert in the empty string
    my_str_clear(&string1);
    my_str_from_cstr(&string2, "hello", 20);
    in_code = my_str_insert(&string1, &string2, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello");
    ASSERT_EQ(my_str_size(&string1), 5);
    ASSERT_EQ(my_str_size(&string2), 5);

    // insert empty string
    my_str_clear(&string2);
    my_str_from_cstr(&string1, "hello", 20);
    in_code = my_str_insert(&string1, &string2, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello");
    ASSERT_EQ(my_str_size(&string1), 5);
    ASSERT_EQ(my_str_size(&string2), 0);

    // one of strings is NULL
    in_code = my_str_insert(NULL, &string2, 0);
    ASSERT_EQ(in_code, NULL_PTR_ERR);
    in_code = my_str_insert(&string1, NULL, 0);
    ASSERT_EQ(in_code, NULL_PTR_ERR);

    // bad position
    in_code = my_str_insert(&string1, &string2, 20);
    ASSERT_EQ(in_code, RANGE_ERR);
    in_code = my_str_insert(&string1, &string2, -20);
    ASSERT_EQ(in_code, RANGE_ERR);
}

TEST_F(ClassDeclaration, my_str_insert_cstr) {
    my_str_from_cstr(&string1, "herld", 20);
    char c_string[10] = "llo, wo";
    char c_string_long[50];
    // normal insert
    int in_code = my_str_insert_cstr(&string1, c_string, 2);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_EQ(test_c_str_len(c_string), 7);

    //insert on the end of string
    my_str_from_cstr(&string1, "hello, ", 20);
    sprintf(c_string_long, "world, how is it going? for me it is just a test");
    in_code = my_str_insert_cstr(&string1, c_string_long, 7);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world, how is it going? for me it is just a test");
    ASSERT_EQ(my_str_size(&string1), 55);
    ASSERT_EQ(test_c_str_len(c_string_long), 48);

    // insert at the end of string
    my_str_from_cstr(&string1, "world", 20);
    sprintf(c_string, "hello, ");
    in_code = my_str_insert_cstr(&string1, c_string, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_EQ(test_c_str_len(c_string), 7);

    // insert in the empty string
    my_str_clear(&string1);
    sprintf(c_string, "hello");
    in_code = my_str_insert_cstr(&string1, c_string, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello");
    ASSERT_EQ(my_str_size(&string1), 5);
    ASSERT_EQ(test_c_str_len(c_string), 5);

    // insert empty string
    sprintf(c_string, "");
    my_str_from_cstr(&string1, "hello", 20);
    in_code = my_str_insert_cstr(&string1, c_string, 0);
    ASSERT_EQ(in_code, 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello");
    ASSERT_EQ(my_str_size(&string1), 5);
    ASSERT_EQ(test_c_str_len(c_string), 0);

    // one of strings is NULL
    in_code = my_str_insert_cstr(NULL, c_string, 0);
    ASSERT_EQ(in_code, NULL_PTR_ERR);
    in_code = my_str_insert_cstr(&string1, NULL, 0);
    ASSERT_EQ(in_code, NULL_PTR_ERR);

    // bad position
    in_code = my_str_insert_cstr(&string1, c_string, 20);
    ASSERT_EQ(in_code, RANGE_ERR);
    in_code = my_str_insert_cstr(&string1, c_string, -20);
    ASSERT_EQ(in_code, RANGE_ERR);
}

TEST_F(ClassDeclaration, my_str_append) {
    my_str_from_cstr(&string1, "hello, ", 20);
    my_str_from_cstr(&string2, "world", 20);

    // normal append
    ASSERT_EQ(my_str_append(&string1, &string2), 0);
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // append empty string
    my_str_clear(&string2);
    ASSERT_EQ(my_str_append(&string1, &string2), 0);
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // append to empty string
    ASSERT_EQ(my_str_append(&string2, &string1), 0);
    ASSERT_EQ(my_str_size(&string2), 12);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello, world");

    // one of string is NULL
    ASSERT_EQ(my_str_append(NULL, &string1), NULL_PTR_ERR);
    ASSERT_EQ(my_str_append(&string2, NULL), NULL_PTR_ERR);

    // big str append
    my_str_from_cstr(&string2, "world, how is it going? for me it is just a test", 60);

    ASSERT_EQ(my_str_append(&string1, &string2), 0);

}

TEST_F(ClassDeclaration, my_str_append_cstr) {
    my_str_from_cstr(&string1, "hello, ", 20);
    char c_string[20] = "world";
    // normal append
    ASSERT_EQ(my_str_append_cstr(&string1, c_string), 0);
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // append empty string
    sprintf(c_string, "");
    ASSERT_EQ(my_str_append_cstr(&string1, c_string), 0);
    ASSERT_EQ(my_str_size(&string1), 12);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // append to empty string
    sprintf(c_string, "hello");
    my_str_clear(&string1);
    ASSERT_EQ(my_str_append_cstr(&string1, c_string), 0);
    ASSERT_EQ(my_str_size(&string2), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "");

    // one of string is NULL
    ASSERT_EQ(my_str_append_cstr(NULL, c_string), NULL_PTR_ERR);
    ASSERT_EQ(my_str_append_cstr(&string2, NULL), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_substr) {
    my_str_from_cstr(&string1, "hello, world. My name is Myk0la and today I want to test your lab work!", 80);

    // substring inside
    ASSERT_EQ(my_str_substr(&string1, &string2, 2, 5), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "llo");
    ASSERT_EQ(my_str_size(&string2), 3);

    // substring from beginning
    ASSERT_EQ(my_str_substr(&string1, &string2, 0, 5), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello");
    ASSERT_EQ(my_str_size(&string2), 5);

    // substring from end
    ASSERT_EQ(my_str_substr(&string1, &string2, 7, 12), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "world");
    ASSERT_EQ(my_str_size(&string2), 5);

    // bad boundaries
    ASSERT_EQ(my_str_substr(&string1, &string2, 13, 11), RANGE_ERR);
    ASSERT_EQ(my_str_substr(&string1, &string2, 7, 20), 0);
    ASSERT_EQ(my_str_substr(&string1, &string2, -20, 11), RANGE_ERR);

    // one of strings is NULL
    ASSERT_EQ(my_str_substr(NULL, &string2, 1, 2), NULL_PTR_ERR);
    ASSERT_EQ(my_str_substr(&string1, NULL, 1, 2), NULL_PTR_ERR);

    // end - beg > capacity 2
    ASSERT_EQ(my_str_substr(&string1, &string2, 0, 90), 0);

    // beg == end
    int errcode = my_str_substr(&string1, &string2, 1, 1);
    ASSERT_EQ(errcode, 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "");

}

TEST_F(ClassDeclaration, my_str_substr_cstr) {
    my_str_from_cstr(&string1, "hello, world", 20);
    char c_string[20] = "a";

    // substring inside
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, 2, 5), 0);
    ASSERT_STREQ(c_string, "llo");
    ASSERT_EQ(test_c_str_len(c_string), 3);

    // substring from beginning
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, 0, 5), 0);
    ASSERT_STREQ(c_string, "hello");
    ASSERT_EQ(test_c_str_len(c_string), 5);

    // substring from end
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, 7, 12), 0);
    ASSERT_STREQ(c_string, "world");
    ASSERT_EQ(test_c_str_len(c_string), 5);

    // bad boundaries
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, 13, 11), RANGE_ERR);
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, 7, 20), 0);
    ASSERT_EQ(my_str_substr_cstr(&string1, c_string, -20, 11), RANGE_ERR);
    ASSERT_EQ(my_str_size(&string1), 12);

    // one of strings is NULL
    ASSERT_EQ(my_str_substr_cstr(NULL, c_string, 1, 2), NULL_PTR_ERR);
    ASSERT_EQ(my_str_substr_cstr(&string1, NULL, 1, 2), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_erase) {
    my_str_from_cstr(&string1, "hello_world", 20);

    // erase inside
    ASSERT_EQ(my_str_erase(&string1, 5, 8), 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hellorld");

    // erase from beginning
    ASSERT_EQ(my_str_erase(&string1, 0, 2), 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "llorld");

    // erase to the end
    ASSERT_EQ(my_str_erase(&string1, 3, 6), 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "llo");

    // bad boundaries
    // too big beg
    ASSERT_EQ(my_str_erase(&string1, 13, 14), RANGE_ERR);
    // beg < 0
    ASSERT_EQ(my_str_erase(&string1, -1, 20), RANGE_ERR);
    // end > size
    ASSERT_EQ(my_str_erase(&string1, 1, 10), 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "l");

    // string is NULL
    ASSERT_EQ(my_str_erase(NULL, 0, 1), NULL_PTR_ERR);
}


TEST_F(ClassDeclaration, my_str_reserve) {
    my_str_from_cstr(&string1, "hello, world", 20);

    // reserve more capacity
    ASSERT_EQ(my_str_reserve(&string1, 40), 0);
    ASSERT_EQ(my_str_capacity(&string1), 40);
    ASSERT_EQ(my_str_reserve(&string1, 80), 0);
    ASSERT_EQ(my_str_capacity(&string1), 80);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // reserve less
    ASSERT_EQ(my_str_reserve(&string1, 40), 0);
    ASSERT_EQ(my_str_reserve(&string1, 79), 0);
    ASSERT_EQ(my_str_reserve(&string1, 80), 0);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");


    // if string is NULL
    ASSERT_EQ(my_str_reserve(NULL, 20), NULL_PTR_ERR);

    // if reserve max possible memory
    size_t s = SIZE_MAX;
    int errcode = my_str_reserve(&string3, s);
    ASSERT_EQ(errcode, MEMORY_ALLOCATION_ERR);
}

TEST_F(ClassDeclaration, my_str_shrink_to_fit) {
    my_str_from_cstr(&string1, "hello, world", 20);

    // normal shrinl
    ASSERT_EQ(my_str_shrink_to_fit(&string1), 0);
    ASSERT_EQ(my_str_capacity(&string1), my_str_size(&string1));
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world");

    // shrink empty string
    my_str_clear(&string1);
    ASSERT_EQ(my_str_shrink_to_fit(&string1), 0);
    ASSERT_EQ(my_str_capacity(&string1), 0);

    // string is NULL
    ASSERT_EQ(my_str_shrink_to_fit(NULL), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_resize) {
    my_str_from_cstr(&string1, "hello, world", 20);

    // bigger size
    ASSERT_EQ(my_str_resize(&string1, 21, '!'), 0);
    ASSERT_EQ(my_str_size(&string1), 21);
    ASSERT_EQ(my_str_capacity(&string1), 42);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello, world!!!!!!!!!");

    //smaller size
    ASSERT_EQ(my_str_resize(&string1, 5, 'a'), 0);
    ASSERT_EQ(my_str_size(&string1), 5);
    ASSERT_EQ(my_str_capacity(&string1), 42);
    ASSERT_STREQ(my_str_get_cstr(&string1), "hello");

    // size = 0
    ASSERT_EQ(my_str_resize(&string1, 0, 'a'), 0);
    ASSERT_EQ(my_str_size(&string1), 0);
    ASSERT_EQ(my_str_capacity(&string1), 42);
    ASSERT_STREQ(my_str_get_cstr(&string1), "");

    // string is NULL
    ASSERT_EQ(my_str_resize(NULL, 1, 'a'), NULL_PTR_ERR);

    // if reserve max possible memory
    size_t s = SIZE_MAX;
    int errcode = my_str_resize(&string1, s, ' ');
    ASSERT_EQ(errcode, MEMORY_ALLOCATION_ERR);
}

TEST_F(ClassDeclaration, my_str_find) {
    my_str_from_cstr(&string1, "hello, world", 20);
    my_str_from_cstr(&string2, "hello", 20);

    // find, from 0 and there is the finding substr
    ASSERT_EQ(my_str_find(&string1, &string2, 0), (size_t) 0);

    // find, from inside and there is the finding substr
    ASSERT_EQ(my_str_find(&string1, &string2, 1), (size_t) NOT_FOUND_CODE);

    // from 0, but there isn't a seeking substr
    my_str_from_cstr(&string2, "hii", 20);
    ASSERT_EQ(my_str_find(&string1, &string2, 0), (size_t) NOT_FOUND_CODE);

    // from inside but there isn't a seeking substr
    my_str_from_cstr(&string2, "hii", 20);
    ASSERT_EQ(my_str_find(&string1, &string2, 3), (size_t) NOT_FOUND_CODE);

    // find inside
    my_str_from_cstr(&string2, "world", 20);
    ASSERT_EQ(my_str_find(&string1, &string2, 3), (size_t) 7);

    // one of strings is null
    ASSERT_EQ(my_str_find(NULL, &string2, 3), (size_t) NULL_PTR_ERR);
    ASSERT_EQ(my_str_find(&string1, NULL, 3), (size_t) NULL_PTR_ERR);

    // from is too big
    ASSERT_EQ(my_str_find(&string1, &string2, 20), (size_t) NOT_FOUND_CODE);
}

TEST_F(ClassDeclaration, my_str_cmp) {

    // equal size normal strings
    my_str_from_cstr(&string1, "hello", 20);
    my_str_from_cstr(&string2, "hellw", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 1);

    // one is longer
    my_str_from_cstr(&string1, "aaaa", 20);
    my_str_from_cstr(&string2, "aab", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 1);

    // one symbol strings
    my_str_from_cstr(&string1, "a", 20);
    my_str_from_cstr(&string2, "b", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 1);

    // one is empty
    my_str_from_cstr(&string1, "a", 20);
    my_str_from_cstr(&string2, "", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 1);
    my_str_from_cstr(&string1, "", 20);
    my_str_from_cstr(&string2, "b", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), -1);

    // equal
    my_str_from_cstr(&string1, "hello, world", 20);
    my_str_from_cstr(&string2, "hello, world", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 0);

    // empty and equal
    my_str_from_cstr(&string1, "", 20);
    my_str_from_cstr(&string2, "", 20);
    ASSERT_EQ(my_str_cmp(&string1, &string2), 0);

    // one of strings is NULL
    ASSERT_EQ(my_str_cmp(NULL, &string2), NULL_PTR_ERR);
    ASSERT_EQ(my_str_cmp(&string1, NULL), NULL_PTR_ERR);

}

TEST_F(ClassDeclaration, my_str_cmp_cstr) {
    // equal size normal strings
    my_str_from_cstr(&string1, "helw", 20);
    char c_string[20] = "hela";
    int ercode = my_str_cmp_cstr(&string1, c_string);
    ASSERT_EQ(ercode, -1);

    // one is longer
    my_str_from_cstr(&string1, "123", 20);
    sprintf(c_string, "13");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), 1);

    // one symbol strings
    my_str_from_cstr(&string1, "a", 20);
    sprintf(c_string, "b");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), 1);

    // one is empty
    my_str_from_cstr(&string1, "a", 20);
    sprintf(c_string, "");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), 1);
    my_str_from_cstr(&string1, "", 20);
    sprintf(c_string, "b");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), -1);

    // equal
    my_str_from_cstr(&string1, "aaa", 20);
    sprintf(c_string, "aaa");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), 0);

    // empty and equal
    my_str_from_cstr(&string1, "", 20);
    sprintf(c_string, "");
    ASSERT_EQ(my_str_cmp_cstr(&string1, c_string), 0);

    // one of strings is NULL
    ASSERT_EQ(my_str_cmp_cstr(NULL, c_string), NULL_PTR_ERR);
    ASSERT_EQ(my_str_cmp_cstr(&string1, NULL), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_find_c) {
    my_str_from_cstr(&string1, "hello, world", 20);

    // find inside starting with 0
    ASSERT_EQ(my_str_find_c(&string1, 'e', 0), 1);

    // find inside starting with not 0
    ASSERT_EQ(my_str_find_c(&string1, 'l', 3), 3);

    // not find inside starting with 0
    ASSERT_EQ(my_str_find_c(&string1, 'i', 0), (size_t) NOT_FOUND_CODE);

    // not find inside starting with not 9
    ASSERT_EQ(my_str_find_c(&string1, 'm', 5), (size_t) NOT_FOUND_CODE);

    // find from last character
    ASSERT_EQ(my_str_find_c(&string1, 'i', 11), (size_t) NOT_FOUND_CODE);
    ASSERT_EQ(my_str_find_c(&string1, 'd', 11), 11);

    // bad from
    ASSERT_EQ(my_str_find_c(&string1, 'i', 13), (size_t) NOT_FOUND_CODE);
    ASSERT_EQ(my_str_find_c(&string1, 'i', -1), (size_t) NOT_FOUND_CODE);

    //string is NULL
    ASSERT_EQ(my_str_find_c(NULL, 'i', 13), (size_t) NULL_PTR_ERR);
}

int func1(int symbol) {
    return symbol % 2 == 0;
}

int func2(int symbol) {
    return symbol % 5 == 0;
}

int func3(int symbol __attribute__((unused))) {
    return 0;
}

// TODO: try another beg (not 0)
TEST_F(ClassDeclaration, my_str_find_if) {
    my_str_from_cstr(&string1, "hello, world", 20);

    // normal seek
    ASSERT_EQ(my_str_find_if(&string1, 0, func1), 0);
    ASSERT_EQ(my_str_find_if(&string1, 3, func1), 3);
    ASSERT_EQ(my_str_find_if(&string1, 0, func2), 11);

    // not found
    ASSERT_EQ(my_str_find_if(&string1, 0, func3), (size_t) NOT_FOUND_CODE);
    ASSERT_EQ(my_str_find_if(&string1, 6, func3), (size_t) NOT_FOUND_CODE);

    // beg > size
    ASSERT_EQ(my_str_find_if(&string1, 20, func3), (size_t) NOT_FOUND_CODE);

    // one of arguments NULL
    ASSERT_EQ(my_str_find_if(NULL, 0, func2), (size_t) NULL_PTR_ERR);
    ASSERT_EQ(my_str_find_if(&string1, 0, NULL), (size_t) NULL_PTR_ERR);

    // empty string
    my_str_clear(&string1);
    ASSERT_EQ(my_str_find_if(&string1, 0, func1), (size_t) NOT_FOUND_CODE);

}

TEST_F(ClassDeclaration, my_str_write_file) {
    my_str_from_cstr(&string1, "hello, \nworld", 20);
    char path_to_wfile[500];    //  used to be (msvc doesn't compiled) char path_to_wfile[test_c_str_len(FILE_DIR) + 30];
    sprintf(path_to_wfile, "%s%s", FILE_DIR, "/write_file.txt");
    char c_string[20];

    // normal write to file
    // !FILE_DIR is located in SOURCE_DIR/google_tests/test_files!
    FILE *file = fopen(path_to_wfile, "w");
    if (file) {
        ASSERT_EQ(my_str_write_file(&string1, file), 0);
        fclose(file);
    }
    // now read and check content of file
    file = fopen(path_to_wfile, "r");
    if (file) {
        size_t fread_res __attribute__((unused)) = fread(c_string, my_str_size(&string1), 1, file);
        fclose(file);
    }
    c_string[my_str_size(&string1)] = '\0';
    ASSERT_STREQ(my_str_get_cstr(&string1), c_string);
    ASSERT_EQ(my_str_size(&string1), test_c_str_len(c_string));

    // write empty string
    my_str_clear(&string1);
    file = fopen(path_to_wfile, "w");
    if (file) {
        ASSERT_EQ(my_str_write_file(&string1, file), 0);
        fclose(file);
    }
    // now read and check content of file
    file = fopen(path_to_wfile, "r");
    if (file) {
        size_t fread_res __attribute__((unused)) = fread(c_string, my_str_size(&string1), 1, file);
        fclose(file);
    }
    c_string[my_str_size(&string1)] = '\0';
    ASSERT_STREQ(my_str_get_cstr(&string1), c_string);
    ASSERT_EQ(my_str_size(&string1), 0);
    ASSERT_STREQ(c_string, "");

    // one of arguments are null
    ASSERT_EQ(my_str_write_file(NULL, nullptr), NULL_PTR_ERR);
    ASSERT_EQ(my_str_write_file(&string1, NULL), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_read_file) {
    my_str_from_cstr(&string1, "hello, \nworld", 20);
    char path_to_rfile[500];    // char path_to_rfile[test_c_str_len(FILE_DIR) + 30];
    sprintf(path_to_rfile, "%s%s", FILE_DIR, "/read_file.txt");

    // normal read
    FILE *file = fopen(path_to_rfile, "w");
    if (file) {
        my_str_write_file(&string1, file);
        fclose(file);
    }

    file = fopen(path_to_rfile, "r");
    if (file) {
        ASSERT_EQ(my_str_read_file(&string2, file), 0);
        ASSERT_STREQ(my_str_get_cstr(&string1), my_str_get_cstr(&string2));
        ASSERT_EQ(my_str_size(&string1), my_str_size(&string2));
        fclose(file);
    }

    // read from empty file
    my_str_clear(&string1);
    file = fopen(path_to_rfile, "w");
    if (file) {
        my_str_write_file(&string1, file);
        fclose(file);
    }

    file = fopen(path_to_rfile, "r");
    if (file) {

        ASSERT_EQ(my_str_read_file(&string2, file), 0);
        ASSERT_STREQ(my_str_get_cstr(&string2), "");
        ASSERT_EQ(my_str_size(&string2), 0);


        // one of arguments is NULL
        ASSERT_EQ(my_str_read_file(NULL, file), NULL_PTR_ERR);
        ASSERT_EQ(my_str_read_file(&string2, NULL), NULL_PTR_ERR);
        fclose(file);
    }
}

TEST_F(ClassDeclaration, my_str_read_file_delim) {
    my_str_from_cstr(&string1, "hello, world!", 20);
    char path_to_rfile[500]; //char path_to_rfile[test_c_str_len(FILE_DIR) + 30];
    sprintf(path_to_rfile, "%s%s", FILE_DIR, "/read_file.txt");

    FILE *file = fopen(path_to_rfile, "w");
    my_str_write_file(&string1, file);
    if (file)
        fclose(file);

    // read to delimiter inside
    file = fopen(path_to_rfile, "r");
    ASSERT_EQ(my_str_read_file_delim(&string2, file, 'l'), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "he");
    ASSERT_EQ(my_str_size(&string2), 2);
    if (file)
        fclose(file);

    // read to delimiter at the beginning
    file = fopen(path_to_rfile, "r");
    ASSERT_EQ(my_str_read_file_delim(&string2, file, 'h'), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "");
    ASSERT_EQ(my_str_size(&string2), 0);
    if (file)
        fclose(file);
    // read to delimiter at the end
    file = fopen(path_to_rfile, "r");
    ASSERT_EQ(my_str_read_file_delim(&string2, file, '!'), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello, world");
    ASSERT_EQ(my_str_size(&string2), 12);
    if (file)
        fclose(file);
}

TEST_F(ClassDeclaration, my_str_read) {
    my_str_from_cstr(&string1, "hello, world!\n", 20);
    char path_to_rfile[500]; //char path_to_rfile[test_c_str_len(FILE_DIR) + 30];
    sprintf(path_to_rfile, "%s%s", FILE_DIR, "/new_stdin_file.txt");
    FILE *file = fopen(path_to_rfile, "w");
    my_str_write_file(&string1, file);
    if (file)
        fclose(file);

    // in stream is now in the file
    FILE *nstdin = freopen(path_to_rfile, "r", stdin);
    // normal text read
    ASSERT_EQ(my_str_read(&string2), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "hello, world!");
    ASSERT_EQ(my_str_size(&string2), 13);
    if (nstdin)
        fclose(nstdin);

    // read from empty stdin
    my_str_from_cstr(&string1, "\n", 20);
    file = fopen(path_to_rfile, "w");
    my_str_write_file(&string1, file);
    if (file)
        fclose(file);

    nstdin = freopen(path_to_rfile, "r", stdin);
    ASSERT_EQ(my_str_read(&string2), 0);
    ASSERT_STREQ(my_str_get_cstr(&string2), "");
    ASSERT_EQ(my_str_size(&string2), 0);
    if (nstdin)
        fclose(nstdin);

    // string is  NULL
    ASSERT_EQ(my_str_read(NULL), NULL_PTR_ERR);
}

TEST_F(ClassDeclaration, my_str_write) {
    testing::internal::CaptureStdout();

    // normal string
    my_str_from_cstr(&string1, "hello, world!", 20);
    ASSERT_EQ(my_str_write(&string1), 0);
    ASSERT_STREQ(testing::internal::GetCapturedStdout().c_str(), "hello, world!");

    testing::internal::CaptureStdout();
    // empty string
    my_str_clear(&string1);
    ASSERT_EQ(my_str_write(&string1), 0);
    ASSERT_STREQ(testing::internal::GetCapturedStdout().c_str(), "");

    testing::internal::CaptureStdout();
    // string with \n
    my_str_from_cstr(&string1, "hello, \nworld", 20);
    ASSERT_EQ(my_str_write(&string1), 0);
    ASSERT_STREQ(testing::internal::GetCapturedStdout().c_str(), "hello, \nworld");

    // string is null
    ASSERT_EQ(my_str_write(NULL), NULL_PTR_ERR);
}

#pragma GCC diagnostic warning "-Wformat-zero-length"
