#include "base64.h"
#include <stdint.h>

static const char EN_STD[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char EN_URL[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const unsigned char DE_STD[256] = {
    [0 ... 255] = 0x80,
    ['A']=0,['B']=1,['C']=2,['D']=3,['E']=4,['F']=5,
    ['G']=6,['H']=7,['I']=8,['J']=9,['K']=10,['L']=11,
    ['M']=12,['N']=13,['O']=14,['P']=15,['Q']=16,['R']=17,
    ['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,
    ['a']=26,['b']=27,['c']=28,['d']=29,['e']=30,['f']=31,
    ['g']=32,['h']=33,['i']=34,['j']=35,['k']=36,['l']=37,
    ['m']=38,['n']=39,['o']=40,['p']=41,['q']=42,['r']=43,
    ['s']=44,['t']=45,['u']=46,['v']=47,['w']=48,['x']=49,
    ['y']=50,['z']=51,
    ['0']=52,['1']=53,['2']=54,['3']=55,['4']=56,
    ['5']=57,['6']=58,['7']=59,['8']=60,['9']=61,
    ['+']=62,['/']=63,
    ['=']=0xFE
};

static const unsigned char DE_URL[256] = {
    [0 ... 255] = 0x80,
    ['A']=0,['B']=1,['C']=2,['D']=3,['E']=4,['F']=5,
    ['G']=6,['H']=7,['I']=8,['J']=9,['K']=10,['L']=11,
    ['M']=12,['N']=13,['O']=14,['P']=15,['Q']=16,['R']=17,
    ['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,
    ['a']=26,['b']=27,['c']=28,['d']=29,['e']=30,['f']=31,
    ['g']=32,['h']=33,['i']=34,['j']=35,['k']=36,['l']=37,
    ['m']=38,['n']=39,['o']=40,['p']=41,['q']=42,['r']=43,
    ['s']=44,['t']=45,['u']=46,['v']=47,['w']=48,['x']=49,
    ['y']=50,['z']=51,
    ['0']=52,['1']=53,['2']=54,['3']=55,['4']=56,
    ['5']=57,['6']=58,['7']=59,['8']=60,['9']=61,
    ['-']=62,
    ['_']=63,
    ['=']=0xFE
};

static inline int is_ws(char c)
{
    return c==' ' || c=='\n' || c=='\r' || c=='\t';
}

int base64_encode(const unsigned char *in, size_t n,
                  char *out, size_t cap)
{
    size_t i, j = 0;

    if (!in || !out) return B64_ERR_PARAM;
    if (cap < ((n + 2) / 3) * 4) return B64_ERR_BUF;

    for (i = 0; i < n; i += 3) {
        unsigned int v = in[i] << 16;

        if (i + 1 < n) v |= in[i + 1] << 8;
        if (i + 2 < n) v |= in[i + 2];

        out[j++] = EN_STD[(v >> 18) & 63];
        out[j++] = EN_STD[(v >> 12) & 63];

        if (i + 1 < n) out[j++] = EN_STD[(v >> 6) & 63];
        else out[j++] = '=';

        if (i + 2 < n) out[j++] = EN_STD[v & 63];
        else out[j++] = '=';
    }

    return (int)j;
}

int base64url_encode(const unsigned char *in, size_t n,
                     char *out, size_t cap)
{
    size_t i, j = 0;

    if (!in || !out) return B64_ERR_PARAM;
    if (cap < ((n + 2) / 3) * 4) return B64_ERR_BUF;

    for (i = 0; i < n; i += 3) {
        unsigned int v = in[i] << 16;

        if (i + 1 < n) v |= in[i + 1] << 8;
        if (i + 2 < n) v |= in[i + 2];

        out[j++] = EN_URL[(v >> 18) & 63];
        out[j++] = EN_URL[(v >> 12) & 63];

        if (i + 1 < n) out[j++] = EN_URL[(v >> 6) & 63];
        else out[j++] = '=';

        if (i + 2 < n) out[j++] = EN_URL[v & 63];
        else out[j++] = '=';
    }

    return (int)j;
}

static int decode_core(const char *in, size_t n,
                      unsigned char *out, size_t cap,
                      size_t *outlen,
                      const unsigned char *map)
{
    size_t i, o = 0;
    unsigned int v = 0;
    int valb = -8;
    int pad = 0;

    for (i = 0; i < n; i++) {
        unsigned char c = in[i];

        if (is_ws(c)) continue;

        unsigned char d = map[c];

        if (d == 0x80) return B64_ERR_CHAR;

        if (d == 0xFE) {
            pad = 1;
            continue;
        }

        if (pad) return B64_ERR_PADDING;

        v = (v << 6) | d;
        valb += 6;

        if (valb >= 0) {
            if (o >= cap) return B64_ERR_BUF;
            out[o++] = (v >> valb) & 0xFF;
            valb -= 8;
        }
    }

    if (outlen) *outlen = o;
    return B64_OK;
}

int base64_decode(const char *in, size_t n,
                  unsigned char *out, size_t cap,
                  size_t *outlen)
{
    return decode_core(in, n, out, cap, outlen, DE_STD);
}

int base64url_decode(const char *in, size_t n,
                     unsigned char *out, size_t cap,
                     size_t *outlen)
{
    return decode_core(in, n, out, cap, outlen, DE_URL);
}

void base64_stream_init(base64_stream_t *s, int urlsafe)
{
    s->val = 0;
    s->valb = -8;
    s->pad = 0;
    s->map = urlsafe ? DE_URL : DE_STD;
}

int base64_stream_feed(base64_stream_t *s,
                       const char *in, size_t n,
                       unsigned char *out, size_t cap,
                       size_t *outlen)
{
    size_t i, o = 0;

    for (i = 0; i < n; i++) {
        unsigned char c = in[i];

        if (is_ws(c)) continue;

        unsigned char d = s->map[c];

        if (d == 0x80) return B64_ERR_CHAR;

        if (d == 0xFE) {
            s->pad = 1;
            continue;
        }

        if (s->pad) return B64_ERR_PADDING;

        s->val = (s->val << 6) | d;
        s->valb += 6;

        if (s->valb >= 0) {
            if (o >= cap) return B64_ERR_BUF;
            out[o++] = (s->val >> s->valb) & 0xFF;
            s->valb -= 8;
        }
    }

    if (outlen) *outlen = o;
    return B64_OK;
}