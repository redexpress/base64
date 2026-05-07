#ifndef BASE64_H
#define BASE64_H

#define B64_ERR_PARAM -1
#define B64_ERR_CHAR -2

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

// out is null-terminated encode string.
// return values is out length, exclusive terminating `\0'

unsigned int
base64_encode(const unsigned char *in, unsigned int inlen, char *out);

unsigned int
base64url_encode(const unsigned char *in, unsigned int inlen, char *out);

// return out length or errcode (< 0)

unsigned int
base64_decode(const char *in, unsigned int inlen, unsigned char *out);

unsigned int
base64url_decode(const char *in, unsigned int inlen, unsigned char *out);

#endif