/**
  Copyright © 2015 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include "md4.h"

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))

/************************************************
 *
 * Transform block of data.
 *
 ************************************************/
void MD4_Transform (MD4_CTX *c) 
{
  uint32_t i, t;
  uint32_t s[4];
  
  // increment by 4
  uint8_t rotf[] = { 3, 7, 11, 19 };
  uint8_t rotg[] = { 3, 5,  9, 13 };
  uint8_t roth[] = { 3, 9, 11, 15 };

  // increment by 4 mod 15
  uint8_t idxg[] = 
  { 0, 4,  8, 12, 
    1, 5,  9, 13, 
    2, 6, 10, 14,  
    3, 7, 11, 15 };
    
  // increment by 8 mod 12, 18, 12, 21
  uint8_t idxh[] = 
  { 0,  8, 4, 12, 
    2, 10, 6, 14, 
    1,  9, 5, 13,  
    3, 11, 7, 15 };

  for (i=0; i<4; i++) {
    s[i]=c->s.w[i];
  }
  
  // for 48 rounds
  for (i=0; i<48; i++) {
    if (i < 16) {
      s[0] += F(s[1], s[2], s[3]) + 
        c->buf.w[i];
      t = rotf[i%4];
    } else if (i < 32) {
      s[0] += G(s[1], s[2], s[3]) + 
        c->buf.w[idxg[i%16]] + 0x5a827999L;
      t = rotg[i%4];
    } else {
      s[0] += H(s[1], s[2], s[3]) + 
        c->buf.w[idxh[i%16]] + 0x6ed9eba1L;
      t = roth[i%4];
    }
    t=ROTL32(s[0], t);
    s[0]=s[3];
    s[3]=s[2];
    s[2]=s[1];
    s[1]=t;
  }

  for (i=0; i<4; i++) {
    c->s.w[i] += s[i];
  }
}

/************************************************
 *
 * initialize state
 *
 ************************************************/
void MD4_Init (MD4_CTX *c) {
  c->len  = 0;
  c->s.w[0] = 0x67452301;
  c->s.w[1] = 0xefcdab89;
  c->s.w[2] = 0x98badcfe;
  c->s.w[3] = 0x10325476;
}

/************************************************
 *
 * Update state
 *
 ************************************************/
void MD4_Update (MD4_CTX *c, void *in, uint32_t len) {
  uint8_t *p = (uint8_t*)in;
  uint32_t r, idx;
  
  if (len==0) return;
  
  // get buffer index
  idx = c->len & MD4_CBLOCK - 1;
  
  // update length
  c->len += len;
  
  while (len > 0) {
    r = MIN (len, MD4_CBLOCK - idx);
    memcpy ((void*)&c->buf.b[idx], p, r);
    if ((idx + r) < MD4_CBLOCK) break;
    
    MD4_Transform (c);
    len -= r;
    idx = 0;
    p += r;
  }
}

/************************************************
 *
 * Finalize.
 *
 ************************************************/
void MD4_Final (void* dgst, MD4_CTX *c)
{
  // see what length we have ere..
  uint32_t len=c->len & MD4_CBLOCK - 1;
  // fill with zeros
  memset (&c->buf.b[len], 0, MD4_CBLOCK - len);
  // add the end bit
  c->buf.b[len] = 0x80;
  // if exceeding 56 bytes, transform it
  if (len >= 56) {
    MD4_Transform (c);
    // clear
    memset (c->buf.b, 0, MD4_CBLOCK);
  }
  // add total bits
  c->buf.q[7] = c->len * 8;
  // compress
  MD4_Transform(c);
  // copy digest to buffer
  memcpy (dgst, c->s.b, MD4_DIGEST_LENGTH);
}
