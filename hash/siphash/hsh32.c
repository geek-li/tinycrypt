
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

void XSIPROUND(uint32_t v[4])
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

uint32_t hsh32(
    const void *data, 
    int inlen, 
    const void *key) 
{
    uint32_t v[4];
    int i, j, len, last;
    uint32_t m;
    uint8_t *in = (uint8_t*)data;
    uint32_t *k = (uint32_t*)key;
    
    uint32_t k0 = k[0];
    uint32_t k1 = k[1];
        
    uint32_t b = inlen << 24;
    
    v[0] = 0;
    v[1] = 0;
    v[2] = 0x6c796765;
    v[3] = 0x74656462;
    
    v[3] ^= k1;
    v[2] ^= k0;
    v[1] ^= k1;
    v[0] ^= k0;
   
    for (last=0;last!=1;)
    {
      // 1. are we less than 4 bytes?  
      len = (inlen < 4) ? inlen : 4;
      if (len < 4) {
        // 2. yes, move length << 24 into m
        m = b;
        last=1;
      } else {
        // 3. no, set m to zero
        m = 0;
      }
      // 4. while len is greater than or equal to zero
      while (--len >= 0) {
        // 5. get byte shifted left by 8 * len
        // this is only performed when input available
        if (last) {
          m |= ((uint32_t)in[len]) << (8 * len);
        } else {
          m |= ((uint32_t)*in) << (8 * len);
          in++;
          inlen--;
        }
      }
      // 6. swap bytes
      if (!last) m = _byteswap_ulong(m);
      
      //printf ("\nv2: %08X", m);
      
      v[3] ^= m;

      for (i=0; i<cROUNDS; i++) {
        XSIPROUND(v);
      }

      v[0] ^= m;
    }
      
    v[2] ^= 0xff;

    for (i = 0; i < dROUNDS; ++i)
        XSIPROUND(v);

    return v[1] ^ v[3];
}
