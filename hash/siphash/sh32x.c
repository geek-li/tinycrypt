
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* default: SipHash-2-4 */
#define cROUNDS 2
#define dROUNDS 4

#define ROTL(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))

void RSIPROUND(uint32_t v[4])
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

int sh32x(
  const uint8_t *data, 
  const size_t inlen, 
  const uint8_t *key) 
{
    uint32_t v[4];
    int i;
    uint32_t m;
    uint8_t *in = (uint8_t*)data;
    uint32_t *k = (uint32_t*)key;
    
    uint32_t k0 = k[0];
    uint32_t k1 = k[1];
        
    const uint8_t *end = in + inlen - (inlen & 3);
    int left = inlen & 3;
    uint32_t b = ((uint32_t)inlen) << 24;
    
    v[0] = k0;
    v[1] = k1;
    v[2] = k0 ^ 0x6c796765;
    v[3] = k1 ^ 0x74656462;

    for (; in != end; in += 4) {
      m = *(uint32_t*)in;
      v[3] ^= m;

      for (i = 0; i < cROUNDS; ++i)
        RSIPROUND(v);

      v[0] ^= m;
    }

    while (--left >= 0) {
      b |= ((uint32_t)in[left]) << (8 * left);   
    }

    v[3] ^= b;

    for (i = 0; i < cROUNDS; ++i)
      RSIPROUND(v);

    v[0] ^= b;
    v[2] ^= 0xff;

     for (i = 0; i < dROUNDS; ++i)
       RSIPROUND(v);

    return v[1] ^ v[3];
}
