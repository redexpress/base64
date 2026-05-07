# Base64 Encoder/Decoder

A simple C library for Base64 encoding and decoding.

## Features

- Standard Base64 encoding/decoding
- URL-safe Base64 encoding/decoding
- C99 style with designated initializers

## API

### Encoding

```c
unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out);
unsigned int base64url_encode(const unsigned char *in, unsigned int inlen, char *out);
```

### Decoding

```c
unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out);
unsigned int base64url_decode(const char *in, unsigned int inlen, unsigned char *out);
```

Returns decoded length on success, or negative error code on failure.

### Error Codes

- `B64_ERR_PARAM`: Invalid parameter
- `B64_ERR_CHAR`: Invalid character in input

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
