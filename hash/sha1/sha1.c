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
  
#include "sha1.h"

/************************************************
*
* transform a block of data.
*
************************************************/
void SHA1_Transform (SHA1_CTX *ctx) 
{
  uint32_t a, b, c, d, e, t, i;
  uint32_t w[80], s[SHA1_LBLOCK];

  // copy buffer to local
  for (i=0; i<16; i++) {
    w[i] = SWAP32(ctx->buf.w[i]);
  }
  
  // expand it
  for (i=16; i<80; i++) {
    w[i] = ROTL32(w[i -  3] ^ 
                  w[i -  8] ^ 
                  w[i - 14] ^ 
                  w[i - 16], 1);
  }
  
  // load state
  memcpy (s, ctx->s.b, SHA1_DIGEST_LENGTH);
  
  // for 80 rounds
  for (i=0; i<80; i++) {
    if (i<20) {
      t = (s[3] ^ (s[1] & (s[2] ^ s[3]))) + 0x5A827999L;
    } else if (i<40) {
      t = (s[1] ^ s[2] ^ s[3]) + 0x6ED9EBA1L;
    } else if (i<60) {
      t = ((s[1] & s[2]) | (s[3] & (s[1] | s[2]))) + 0x8F1BBCDCL;
    } else {
      t = (s[1] ^ s[2] ^ s[3]) + 0xCA62C1D6L;
    }
    t += ROTL32(s[0], 5) + s[4] + w[i];
    s[4] = s[3];
    s[3] = s[2];
    s[2] = ROTL32(s[1], 30);
    s[1] = s[0];
    s[0] = t;
  }
  
  // update state
  for (i=0; i<SHA1_LBLOCK; i++) {
    ctx->s.w[i] += s[i];
  }
}

/************************************************
*
* initialize context
*
************************************************/
void SHA1_Init (SHA1_CTX *c) {
  c->len    = 0;
  c->s.w[0] = 0x67452301;
  c->s.w[1] = 0xefcdab89;
  c->s.w[2] = 0x98badcfe;
  c->s.w[3] = 0x10325476;
  c->s.w[4] = 0xc3d2e1f0;
}

/************************************************
*
* update state with input
*
************************************************/
void SHA1_Update (SHA1_CTX *c, void *in, uint32_t len) {
  uint8_t *p = (uint8_t*)in;
  uint32_t  r, idx;
  
  if (len==0) return;
  
  // get buffer index
  idx = c->len & SHA1_CBLOCK - 1;
  
  // update length
  c->len += len;
  
  while (len > 0) {
    r = MIN(len, SHA1_CBLOCK - idx);
    memcpy ((void*)&c->buf.b[idx], p, r);
    if ((idx + r) < SHA1_CBLOCK) break;
    
    SHA1_Transform (c);
    len -= r;
    idx = 0;
    p += r;
  }
}

/************************************************
*
* finalize.
*
************************************************/
void SHA1_Final (void* dgst, SHA1_CTX *c)
{
  uint32_t i;
  
  // see what length we have ere..
  uint32_t len=c->len & SHA1_CBLOCK - 1;
  
  // add the end bit
  c->buf.b[len++] = 0x80;
  
  // fill remaining space with zeros
  memset (&c->buf.b[len], 0, SHA1_CBLOCK - len);
  
  // if exceeding 56 bytes, transform it
  if (len >= 56) {
    SHA1_Transform (c);
    // clear buffer
    memset (c->buf.b, 0, SHA1_CBLOCK);
  }
  // add total bits
  c->buf.q[7] = SWAP64(c->len << 3);
  
  // compress
  SHA1_Transform (c);
  
  // swap byte order
  for (i=0; i<SHA1_LBLOCK; i++) {
    c->s.w[i] = SWAP32(c->s.w[i]);
  }
  // copy digest to buffer
  memcpy (dgst, c->s.b, SHA1_DIGEST_LENGTH);
}
