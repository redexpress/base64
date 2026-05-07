#include <criterion/criterion.h>
#include "../base64.h"  // Adjust path if needed

// Test base64_encode
Test(base64_encode, empty_input) {
    unsigned char in[] = "";
    char out[10];
    int ret = base64_encode(in, 0, out, sizeof(out));
    cr_assert_eq(ret, 0);
    cr_assert_str_empty(out);
}

Test(base64_encode, single_byte) {
    unsigned char in[] = "f";
    char out[10];
    int ret = base64_encode(in, 1, out, sizeof(out));
    cr_assert_eq(ret, 4);
    cr_assert_str_eq(out, "Zg==");
}

Test(base64_encode, hello_world) {
    unsigned char in[] = "Hello World";
    char out[20];
    int ret = base64_encode(in, 11, out, sizeof(out));
    cr_assert_eq(ret, 16);
    cr_assert_str_eq(out, "SGVsbG8gV29ybGQ=");
}

Test(base64_encode, buffer_too_small) {
    unsigned char in[] = "Hello";
    char out[3];
    int ret = base64_encode(in, 5, out, sizeof(out));
    cr_assert_eq(ret, B64_ERR_BUF);
}

// Test base64url_encode
Test(base64url_encode, hello_world) {
    unsigned char in[] = "Hello World";
    char out[20];
    int ret = base64url_encode(in, 11, out, sizeof(out));
    cr_assert_eq(ret, 16);
    cr_assert_str_eq(out, "SGVsbG8gV29ybGQ=");
}

Test(base64url_encode, with_padding) {
    unsigned char in[] = "f";
    char out[10];
    int ret = base64url_encode(in, 1, out, sizeof(out));
    cr_assert_eq(ret, 4);
    cr_assert_str_eq(out, "Zg==");
}

// Test base64_decode
Test(base64_decode, empty_input) {
    char in[] = "";
    unsigned char out[10];
    size_t outlen;
    int ret = base64_decode(in, 0, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_OK);
    cr_assert_eq(outlen, 0);
}

Test(base64_decode, hello_world) {
    char in[] = "SGVsbG8gV29ybGQ=";
    unsigned char out[20];
    size_t outlen;
    int ret = base64_decode(in, 16, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_OK);
    cr_assert_eq(outlen, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

Test(base64_decode, invalid_char) {
    char in[] = "SGVs@G8gV29ybGQ=";
    unsigned char out[20];
    size_t outlen;
    int ret = base64_decode(in, 16, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_ERR_CHAR);
}

Test(base64_decode, incorrect_padding) {
    char in[] = "SGVsbG8gV29ybGQ";
    unsigned char out[20];
    size_t outlen;
    int ret = base64_decode(in, 15, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_ERR_PADDING);
}

Test(base64_decode, buffer_too_small) {
    char in[] = "SGVsbG8gV29ybGQ=";
    unsigned char out[5];
    size_t outlen;
    int ret = base64_decode(in, 16, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_ERR_BUF);
}

// Test base64url_decode
Test(base64url_decode, hello_world) {
    char in[] = "SGVsbG8gV29ybGQ=";
    unsigned char out[20];
    size_t outlen;
    int ret = base64url_decode(in, 16, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_OK);
    cr_assert_eq(outlen, 11);
    cr_assert_arr_eq(out, "Hello World", 11);
}

// Test stream functions
Test(base64_stream, init_and_feed) {
    base64_stream_t s;
    base64_stream_init(&s, 0);  // standard
    char in[] = "SGVs";
    unsigned char out[10];
    size_t outlen;
    int ret = base64_stream_feed(&s, in, 4, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_OK);
    cr_assert_eq(outlen, 3);
    cr_assert_arr_eq(out, "Hel", 3);
}

Test(base64_stream, urlsafe) {
    base64_stream_t s;
    base64_stream_init(&s, 1);  // urlsafe
    char in[] = "SGVs";
    unsigned char out[10];
    size_t outlen;
    int ret = base64_stream_feed(&s, in, 4, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_OK);
    cr_assert_eq(outlen, 3);
    cr_assert_arr_eq(out, "Hel", 3);
}

// Test error cases
Test(base64_encode, null_input) {
    char out[10];
    int ret = base64_encode(NULL, 1, out, sizeof(out));
    cr_assert_eq(ret, B64_ERR_PARAM);
}

Test(base64_encode, null_output) {
    unsigned char in[] = "a";
    int ret = base64_encode(in, 1, NULL, 10);
    cr_assert_eq(ret, B64_ERR_PARAM);
}

Test(base64_decode, null_input) {
    unsigned char out[10];
    size_t outlen;
    int ret = base64_decode(NULL, 1, out, sizeof(out), &outlen);
    cr_assert_eq(ret, B64_ERR_PARAM);
}

Test(base64_decode, null_output) {
    char in[] = "a";
    size_t outlen;
    int ret = base64_decode(in, 1, NULL, 10, &outlen);
    cr_assert_eq(ret, B64_ERR_PARAM);
}