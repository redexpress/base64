#include "base64.h"

#define BASE64_PAD '='

static const char encode_standard[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char encode_url[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const unsigned char decode_standard[] = {
    [0 ... 31]=255,
    [' ']=32, ['!']=33, ['"']=34, ['#']=35, ['$']=36, ['%']=37, ['&']=38, ['\'']=255,
    ['(']=255,[')']=255,['*']=255,['+']=62, [',']=255,['-']=255,['.']=255,['/']=63,
    ['0']=52, ['1']=53, ['2']=54, ['3']=55, ['4']=56, ['5']=57, ['6']=58, ['7']=59,
    ['8']=60, ['9']=61, [':']=255,[';']=255,['<']=255,['=']=62, ['>']=63, ['?']=255,
    ['@']=255,['A']=0,  ['B']=1,  ['C']=2,  ['D']=3,  ['E']=4,  ['F']=5,  ['G']=6,
    ['H']=7,  ['I']=8,  ['J']=9,  ['K']=10, ['L']=11, ['M']=12, ['N']=13, ['O']=14,
    ['P']=15, ['Q']=16, ['R']=17, ['S']=18, ['T']=19, ['U']=20, ['V']=21, ['W']=22,
    ['X']=23, ['Y']=24, ['Z']=25, ['[']=255,['\\']=255,[']']=255,['^']=255,['_']=255,
    ['`']=255,['a']=26, ['b']=27, ['c']=28, ['d']=29, ['e']=30, ['f']=31, ['g']=32,
    ['h']=33, ['i']=34, ['j']=35, ['k']=36, ['l']=37, ['m']=38, ['n']=39, ['o']=40,
    ['p']=41, ['q']=42, ['r']=43, ['s']=44, ['t']=45, ['u']=46, ['v']=47, ['w']=48,
    ['x']=49, ['y']=50, ['z']=51, ['{']=255,['|']=255,['}']=255,['~']=255,[127]=255
};

static const unsigned char decode_url[] = {
    [0 ... 31]=255,
    [' ']=32, ['!']=33, ['"']=34, ['#']=35, ['$']=36, ['%']=37, ['&']=38, ['\'']=255,
    ['(']=255,[')']=255,['*']=255,['+']=255,[',']=255,['-']=62,['.']=255,['/']=255,
    ['0']=52, ['1']=53, ['2']=54, ['3']=55, ['4']=56, ['5']=57, ['6']=58, ['7']=59,
    ['8']=60, ['9']=61, [':']=255,[';']=255,['<']=255,['=']=62, ['>']=63, ['?']=255,
    ['@']=255,['A']=0,  ['B']=1,  ['C']=2,  ['D']=3,  ['E']=4,  ['F']=5,  ['G']=6,
    ['H']=7,  ['I']=8,  ['J']=9,  ['K']=10, ['L']=11, ['M']=12, ['N']=13, ['O']=14,
    ['P']=15, ['Q']=16, ['R']=17, ['S']=18, ['T']=19, ['U']=20, ['V']=21, ['W']=22,
    ['X']=23, ['Y']=24, ['Z']=25, ['[']=255,['\\']=255,[']']=255,['^']=255,['_']=63,
    ['`']=255,['a']=26, ['b']=27, ['c']=28, ['d']=29, ['e']=30, ['f']=31, ['g']=32,
    ['h']=33, ['i']=34, ['j']=35, ['k']=36, ['l']=37, ['m']=38, ['n']=39, ['o']=40,
    ['p']=41, ['q']=42, ['r']=43, ['s']=44, ['t']=45, ['u']=46, ['v']=47, ['w']=48,
    ['x']=49, ['y']=50, ['z']=51, ['{']=255,['|']=255,['}']=255,['~']=255,[127]=255
};

unsigned int
base64_encode_core(const unsigned char *in, unsigned int inlen, char *out, int base64url)
{
    const char *en = base64url ? encode_url : encode_standard;
    unsigned int j = 0;
    unsigned int s = 0;
    unsigned char l = 0;

    for (unsigned int i = 0; i < inlen; i++) {
        unsigned char c = in[i];

        switch (s) {
        case 0:
            s = 1;
            out[j++] = en[(c >> 2) & 0x3F];
            break;
        case 1:
            s = 2;
            out[j++] = en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
            break;
        case 2:
            s = 0;
            out[j++] = en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
            out[j++] = en[c & 0x3F];
            break;
        }
        l = c;
    }

    switch (s) {
    case 1:
        out[j++] = en[(l & 0x3) << 4];
        if (!base64url) {
            out[j++] = BASE64_PAD;
            out[j++] = BASE64_PAD;
        }
        break;
    case 2:
        out[j++] = en[(l & 0xF) << 2];
        if (!base64url) {
            out[j++] = BASE64_PAD;
        }
        break;
    }

    out[j] = '\0';
    return j;
}

unsigned int
base64_encode(const unsigned char *in, unsigned int inlen, char *out)
{
    return base64_encode_core(in, inlen, out, 0);
}

unsigned int
base64url_encode(const unsigned char *in, unsigned int inlen, char *out)
{
    return base64_encode_core(in, inlen, out, 1);
}

unsigned int
base64_decode_core(const char *in, unsigned int inlen, unsigned char *out, int base64url)
{
    const unsigned char *de = base64url ? decode_url : decode_standard;
    unsigned int j = 0;

    for (unsigned int i = 0; i < inlen; i++) {
        unsigned char c = (unsigned char)in[i];

        if (c == BASE64_PAD) {
            break;
        }
        if (c < 43 || c > 122) {
            return B64_ERR_CHAR;
        }

        c = de[c];
        if (c == 255) {
            return B64_ERR_CHAR;
        }

        switch (i & 0x3) {
        case 0:
            out[j] = (c << 2) & 0xFF;
            break;
        case 1:
            out[j++] |= (c >> 4) & 0x3;
            out[j] = (c & 0xF) << 4;
            break;
        case 2:
            out[j++] |= (c >> 2) & 0xF;
            out[j] = (c & 0x3) << 6;
            break;
        case 3:
            out[j++] |= c;
            break;
        }
    }
    return j;
}

unsigned int
base64_decode(const char *in, unsigned int inlen, unsigned char *out)
{
    return base64_decode_core(in, inlen, out, 0);
}

unsigned int
base64url_decode(const char *in, unsigned int inlen, unsigned char *out)
{
    return base64_decode_core(in, inlen, out, 1);
}