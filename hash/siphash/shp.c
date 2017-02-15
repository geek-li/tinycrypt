
/*
   SipHash reference C implementation

   Copyright (c) 2016 Jean-Philippe Aumasson <jeanphilippe.aumasson@gmail.com>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along
   with
   this software. If not, see
   <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* default: SipHash-2-4 */
#define cROUNDS 2
#define dROUNDS 4

#define ROTL(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))

#define SIPROUND                                                               \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = ROTL(v1, 5);                                                      \
        v1 ^= v0;                                                              \
        v0 = ROTL(v0, 16);                                                     \
        v2 += v3;                                                              \
        v3 = ROTL(v3, 8);                                                      \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = ROTL(v3, 7);                                                      \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = ROTL(v1, 13);                                                     \
        v1 ^= v2;                                                              \
        v2 = ROTL(v2, 16);                                                     \
    } while (0)


int shp(
    const uint8_t *data, 
    const size_t inlen, 
    const uint8_t *key) 
{
    uint32_t v0 = 0;
    uint32_t v1 = 0;
    uint32_t v2 = 0x6c796765;
    uint32_t v3 = 0x74656462;

    uint8_t *in = (uint8_t*)data;
    uint32_t *k = (uint32_t*)key;
    
    uint32_t k0 = k[0];
    uint32_t k1 = k[1];

    uint32_t m;
    int i;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint32_t));
    int left = inlen & 3;
    uint32_t b = ((uint32_t)inlen) << 24;
    
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    for (; in != end; in += 4) {
        m = *(uint32_t*)in;
        v3 ^= m;

        for (i = 0; i < cROUNDS; ++i)
            SIPROUND;

        v0 ^= m;
    }

    while (--left >= 0) {
      b |= ((uint32_t)in[left]) << (8 * left);   
    }

    v3 ^= b;

    for (i = 0; i < cROUNDS; ++i)
        SIPROUND;

    v0 ^= b;
    v2 ^= 0xff;

    for (i = 0; i < dROUNDS; ++i)
        SIPROUND;

    return v1 ^ v3;
}
