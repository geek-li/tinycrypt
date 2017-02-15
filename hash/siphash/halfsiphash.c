
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

#define U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define U8TO32_LE(p)                                                           \
    (((uint32_t)((p)[0])) | ((uint32_t)((p)[1]) << 8) |                        \
     ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24))

void SIPROUND(uint32_t v[4])
{
    v[0] += v[1];                                                              
    v[1] = ROTL(v[1], 5);                                                      
    v[1] ^= v[0];                                                              
    v[0] = ROTL(v[0], 16);                                                     
    v[2] += v[3];                                                              
    v[3] = ROTL(v[3], 8);                                                      
    v[3] ^= v[2];                                                              
    v[0] += v[3];                                                              
    v[3] = ROTL(v[3], 7);                                                      
    v[3] ^= v[0];                                                              
    v[2] += v[1];                                                              
    v[1] = ROTL(v[1], 13);                                                     
    v[1] ^= v[2];                                                              
    v[2] = ROTL(v[2], 16);                                                     
}

#ifdef DEBUG
#define TRACE                                                                  \
    do {                                                                       \
        printf("(%3d) v0 %08x\n", (int)inlen, v0);                             \
        printf("(%3d) v1 %08x\n", (int)inlen, v1);                             \
        printf("(%3d) v2 %08x\n", (int)inlen, v2);                             \
        printf("(%3d) v3 %08x\n", (int)inlen, v3);                             \
    } while (0)
#else
#define TRACE
#endif

int halfsiphash(const uint8_t *in, const size_t inlen, const uint8_t *k,
                uint8_t *out, const size_t outlen) {

    uint32_t v[4];
    int i;
    uint32_t m;
    
    uint32_t k0 = U8TO32_LE(k);
    uint32_t k1 = U8TO32_LE(k + 4);
        
    const uint8_t *end = in + inlen - (inlen % sizeof(uint32_t));
    const int left = inlen & 3;
    uint32_t b = ((uint32_t)inlen) << 24;
    
    v[0] = 0;
    v[1] = 0;
    v[2] = 0x6c796765;
    v[3] = 0x74656462;
    
    v[3] ^= k1;
    v[2] ^= k0;
    v[1] ^= k1;
    v[0] ^= k0;

    if (outlen == 8)
        v[1] ^= 0xee;

    for (; in != end; in += 4) {
        m = U8TO32_LE(in);
        v[3] ^= m;

        printf ("\nv1: %08X", m);
        TRACE;
        for (i = 0; i < cROUNDS; ++i)
            SIPROUND(v);

        v[0] ^= m;
    }

    switch (left) {
    case 3:
        b |= ((uint32_t)in[2]) << 16;
    case 2:
        b |= ((uint32_t)in[1]) << 8;
    case 1:
        b |= ((uint32_t)in[0]);
        break;
    case 0:
        break;
    }

    printf ("\nv1: %08X", b);
    v[3] ^= b;

    TRACE;
    for (i = 0; i < cROUNDS; ++i)
        SIPROUND(v);

    v[0] ^= b;

    if (outlen == 8)
        v[2] ^= 0xee;
    else
        v[2] ^= 0xff;

    TRACE;
    for (i = 0; i < dROUNDS; ++i)
        SIPROUND(v);

    b = v[1] ^ v[3];
    U32TO8_LE(out, b);

    if (outlen == 4)
        return 0;

    v[1] ^= 0xdd;

    TRACE;
    for (i = 0; i < dROUNDS; ++i)
        SIPROUND(v);

    b = v[1] ^ v[3];
    U32TO8_LE(out + 4, b);

    return 0;
}
