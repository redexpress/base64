#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

#define B64_OK          0  // Success
#define B64_ERR_PARAM  -1  // Invalid argument (e.g., NULL pointer)
#define B64_ERR_CHAR   -2  // Invalid Base64 character in input
#define B64_ERR_PADDING -3 // Incorrect padding
#define B64_ERR_BUF    -4  // Output buffer too small

int base64_encode(const unsigned char *in, size_t inlen, char *out, size_t outcap);
int base64url_encode(const unsigned char *in, size_t inlen, char *out, size_t outcap);

int base64_decode(const char *in, size_t inlen, unsigned char *out, size_t outcap, size_t *outlen);
int base64url_decode(const char *in, size_t inlen, unsigned char *out, size_t outcap, size_t *outlen);

typedef struct {
    unsigned int val;
    int valb;
    int pad;
    const unsigned char *map;
} base64_stream_t;

void base64_stream_init(base64_stream_t *s, int urlsafe);

int base64_stream_feed(base64_stream_t *s, const char *in, size_t inlen,
                       unsigned char *out, size_t outcap, size_t *outlen);

#endif