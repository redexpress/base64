#include <criterion/criterion.h>
#include <stdio.h>
#include "../base64.h"

// Test base64_encode
Test(base64_encode, empty_input) {
    unsigned char in[] = "";
    char out[10] = {0};
    int ret = base64_encode(in, 0, out);
    cr_assert_eq(ret, 0);
    cr_assert_str_empty(out);
}

Test(base64_encode, single_byte) {
    unsigned char in[] = "f";
    char out[10] = {0};
    int ret = base64_encode(in, 1, out);
    cr_assert_eq(ret, 4);
    cr_assert_str_eq(out, "Zg==");
}

Test(base64_encode, hello_world) {
    unsigned char in[] = "Hello World";
    char out[20] = {0};
    int ret = base64_encode(in, 11, out);
    cr_assert_eq(ret, 16);
    cr_assert_str_eq(out, "SGVsbG8gV29ybGQ=");
}

Test(base64_encode, buffer_too_small) {
    unsigned char in[] = "Hello";
    char out[3] = {0};
    int ret = base64_encode(in, 5, out);
    cr_assert_eq(ret, 8);
}

// Test base64_decode
Test(base64_decode, empty_input) {
    const char *in = "";
    unsigned char out[10] = {0};
    int ret = base64_decode(in, 0, out);
    cr_assert_eq(ret, 0);
}

Test(base64_decode, hello_world) {
    char in[] = "SGVsbG8gV29ybGQ=";
    unsigned char out[20] = {0};
    int ret = base64_decode(in, 16, out);
    cr_assert_eq(ret, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

Test(base64_decode, invalid_char) {
    char in[] = "SGVs@G8gV29ybGQ=";
    unsigned char out[20] = {0};
    int ret = base64_decode(in, 16, out);
    cr_assert_eq(ret, B64_ERR_CHAR);
}

Test(base64_decode, two_padding) {
    char in[] = "Zg==";
    unsigned char out[20];
    int ret = base64_decode(in, 4, out);
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64_decode, one_padding) {
    char in[] = "Zg=";
    unsigned char out[20];
    int ret = base64_decode(in, 3, out);
   
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64_decode, no_padding) {
    char in[] = "Zg";
    unsigned char out[20];
    int ret = base64_decode(in, 2, out);
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64_decode, invalid_quad_padding) {
    char in[] = "Zg====";
    unsigned char out[20] = {0};
    int ret = base64_decode(in, 6, out);
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64_decode, invalid_padding) {
    char in[] = "YWJj=";
    unsigned char out[20] = {0};
    int ret = base64_decode(in, 5, out);
    cr_assert_eq(ret, 3);
}

// Test base64url_encode
Test(base64url_encode, single_byte) {
    unsigned char in[] = "f";
    char out[10] = {0};
    int ret = base64url_encode(in, 1, out);
    cr_assert_eq(ret, 2);
    cr_assert_str_eq(out, "Zg");
}

Test(base64url_encode, hello_world) {
    unsigned char in[] = "Hello World";
    char out[20] = {0};
    int ret = base64url_encode(in, 11, out);
    cr_assert_eq(ret, 15);
    cr_assert_str_eq(out, "SGVsbG8gV29ybGQ");
}

Test(base64url_encode, uses_dash_underscore) {
    unsigned char in[] = "fo~~";
    char out[10] = {0};
    int ret = base64url_encode(in, 4, out);
    cr_assert_eq(ret, 6);
    cr_assert_str_eq(out, "Zm9-fg");
}

// Test base64url_decode
Test(base64url_decode, hello_world) {
    char in[] = "SGVsbG8gV29ybGQ";
    unsigned char out[20] = {0};
    int ret = base64url_decode(in, 15, out);
    cr_assert_eq(ret, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

Test(base64url_decode, with_padding) {
    char in[] = "Zg==";
    unsigned char out[20];
    int ret = base64url_decode(in, 4, out);
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64url_decode, no_padding) {
    char in[] = "Zg";
    unsigned char out[20];
    int ret = base64url_decode(in, 2, out);
    cr_assert_eq(ret, 1);
    cr_assert_arr_eq(out, "f", 1);
}

Test(base64url_decode, invalid_char) {
    char in[] = "SGVs@G8gV29ybGQ";
    unsigned char out[20] = {0};
    int ret = base64url_decode(in, 15, out);
    cr_assert_eq(ret, B64_ERR_CHAR);
}

// stream
Test(base64_stream, hello_world_single_feed) {
    base64_stream_t s;

    char in[] = "SGVsbG8gV29ybGQ=";
    unsigned char out[20] = {0};
    base64_stream_init(&s, 0);
    int ret = base64_stream_feed(&s, in, 16, out);
    cr_assert_eq(ret, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

Test(base64_stream, hello_world_multi_feed) {
    base64_stream_t s;

    unsigned char out[20] = {0};
    int n;
    int total = 0;
    base64_stream_init(&s, 0);
    n = base64_stream_feed(&s, "SGVs", 4, out + total);
    total += n;
    n = base64_stream_feed(&s, "bG8g", 4, out + total);
    total += n;
    n = base64_stream_feed(&s, "V29y", 4, out + total);
    total += n;
    n = base64_stream_feed(&s, "bGQ=", 4, out + total);
    total += n;
    cr_assert_eq(total, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

Test(base64_stream, one_char_chunks) {
    base64_stream_t s;

    char in[] = "SGVsbG8=";
    unsigned char out[20] = {0};
    int total = 0;
    base64_stream_init(&s, 0);
    for (int i = 0; i < 8; i++) {
        int n = base64_stream_feed(&s, &in[i], 1, out + total);
        total += n;
    }
    cr_assert_eq(total, 5);
    cr_assert_arr_eq(out, "Hello", 5);
}

Test(base64_stream, urlsafe) {
    base64_stream_t s;

    char in[] = "Zm9-fg";
    unsigned char out[20] = {0};
    base64_stream_init(&s, 1);
    int ret = base64_stream_feed(&s, in, 6, out);
    cr_assert_eq(ret, 4);
    cr_assert_arr_eq(out, "fo~~", 4);
}

Test(base64_stream, invalid_char) {
    base64_stream_t s;

    char in[] = "SGVs@A==";
    unsigned char out[20] = {0};
    base64_stream_init(&s, 0);
    int ret = base64_stream_feed(&s, in, 8, out);
    cr_assert_eq(ret, B64_ERR_CHAR);
}
