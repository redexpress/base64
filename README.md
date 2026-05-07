# Base64 Encoder/Decoder

Lightweight Base64/Base64URL codec in C99 with streaming decode and RFC-compliant parsing.

## Features

- Standard Base64 encoding/decoding
- URL-safe Base64 encoding/decoding
- Streaming API support
- C99 style with designated initializers

## API

### Block API

#### Encoding

```c
unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out);
unsigned int base64url_encode(const unsigned char *in, unsigned int inlen, char *out);
```

#### Decoding

```c
unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out);
unsigned int base64url_decode(const char *in, unsigned int inlen, unsigned char *out);
```

Returns decoded length on success, or negative error code on failure.

### Streaming API

```c
typedef struct {
    unsigned int state;
    unsigned int value;
    int base64url;
} base64_stream_t;

void base64_stream_init(base64_stream_t *s, int base64url);
unsigned int base64_stream_feed(base64_stream_t *s, const char *in, unsigned int inlen, unsigned char *out);
```

Example:

```c
base64_stream_t s;
unsigned char out[64];
unsigned int n;

base64_stream_init(&s, 0);

n = base64_stream_feed(&s, "SGVs", 4, out);
fwrite(out, 1, n, stdout);

n = base64_stream_feed(&s, "bG8g", 4, out);
fwrite(out, 1, n, stdout);
// ...

printf("\n");
```

### Error Codes

- `B64_ERR_PARAM`: Invalid parameter
- `B64_ERR_CHAR`: Invalid character in input

### Size Macros

```c
#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))
```

## Build

```bash
gcc -c base64.c -o base64.o
```

## Test

Uses Criterion framework:

```bash
cd tests
gcc -c test_base64.c -o test_base64.o
gcc base64.o test_base64.o -o test_base64 -lcriterion
./test_base64
```
