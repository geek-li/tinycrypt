/**
  Copyright © 2015, 2016 Odzhan. All Rights Reserved.

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

#include "sha3.h"

// without shift functions, 32-bit compiler uses
// external assembly functions
uint64_t shl64 (uint64_t v, uint32_t n)
{
  union {
    uint32_t v32[2];
    uint64_t v64;
  } r, t;
  
  r.v64 = 0;
  
  if (n<64) {
    t.v64 = v;
    if (n==0) {
      r.v64 = t.v64;
    } else if (n<32) {
      r.v32[0]  =  t.v32[0] << n; // shift low bits left by n
      r.v32[1]  =  t.v32[1] << n; // shift upper bits left by n
      r.v32[1] |= (t.v32[0] >> (32 - n));
    } else {
      r.v32[0]  = 0;
      r.v32[1]  = (t.v32[0] << (n & 31));
    }
  }
  return r.v64;
}

uint64_t shr64 (uint64_t v, uint32_t n)
{
  union {
    uint32_t v32[2];
    uint64_t v64;
  } r, t;
  
  r.v64 = 0;
  
  if (n<64) {
    t.v64 = v;
    if (n==0) {
      r.v64 = t.v64;
    } else if (n<32) {
      r.v32[0]  =  t.v32[0] >> n; // shift low bits right by n
      r.v32[1]  =  t.v32[1] >> n; // shift upper bits right by n
      r.v32[0] |= (t.v32[1] << (32 - n));
    } else {
      r.v32[1]  = 0;
      r.v32[0]  = (t.v32[1] >> (n & 31));
    }
  }
  return r.v64;
}

uint64_t XROTL64 (uint64_t v, uint32_t n) 
{
  return shl64(v, n) | shr64(v, (64 - n));
}

// round constant function
// Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1
uint64_t rc (uint8_t *LFSR)
{
  uint64_t c;
  uint32_t i, t;

  c = 0;
  t = *LFSR;
  
  for (i=1; i<128; i += i) 
  {
    if (t & 1) {
      //c ^= (uint64_t)1ULL << (i - 1);
      c ^= shl64 (1, (i - 1));
    }
    t = (t & 0x80) ? (t << 1) ^ 0x71 : t << 1;
  }
  *LFSR = (uint8_t)t;
  return c;
}

void SHA3_Transform (SHA3_CTX *c)
{
  uint32_t i, j, rnd, r;
  uint64_t t, bc[5];
  uint64_t *st=(uint64_t*)c->s.v64;
  uint8_t  lfsr=1;
  
const uint8_t keccakf_piln[24] = 
{ 10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4, 
  15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1  };
  
  for (rnd=0; rnd<SHA3_ROUNDS; rnd++) 
  {
    // Theta
    for (i=0; i<5; i++) {     
      bc[i] = st[i] 
            ^ st[i +  5] 
            ^ st[i + 10] 
            ^ st[i + 15] 
            ^ st[i + 20];
    }
    for (i=0; i<5; i++) {
      t = bc[(i + 4) % 5] ^ XROTL64(bc[(i + 1) % 5], 1);
      for (j=0; j<25; j+=5) {
        st[j + i] ^= t;
      }
    }

    // Rho Pi
    t = st[1];
    for (i=0, r=0; i<24; i++) {
      r += i + 1;
      j = keccakf_piln[i];
      bc[0] = st[j];
      st[j] = XROTL64(t, r & 63);
      t = bc[0];
    }

    // Chi
    for (j=0; j<25; j+=5) {
      for (i=0; i<5; i++) {
        bc[i] = st[j + i];
      }
      for (i=0; i<5; i++) {
        st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
      }
    }
    // Iota
    st[0] ^= rc(&lfsr);
  }
}

// mdlen isn't checked, it presumes caller provides 28,32,48 or 64
void SHA3_Init (SHA3_CTX *c, int mdlen)
{
  uint32_t i;
  
  c->olen = mdlen;
  c->blen = 200 - (2 * mdlen);
  c->idx  = 0;
  
  for (i=0; i<SHA3_STATE_LEN; i++) {
    c->s.v64[i] = 0;
  }
}

void SHA3_Update (SHA3_CTX* c, void *in, uint32_t inlen)
{
  uint32_t i;
  
  // update buffer and state
  for (i=0; i<inlen; i++) {
    // absorb byte into state
    c->s.v8[c->idx++] ^= ((uint8_t*)in)[i];    
    if (c->idx == c->blen) {
      SHA3_Transform (c);
      c->idx = 0;
    }
  }
}

void SHA3_Final (void* out, SHA3_CTX* c)
{
  uint32_t i;
  // absorb 3 bits, Keccak uses 1
  c->s.v8[c->idx] ^= 6;
  // absorb end bit
  c->s.v8[c->blen-1] ^= 0x80;
  // update context
  SHA3_Transform (c);
  // copy digest to buffer
  for (i=0; i<c->olen; i++) {
    ((uint8_t*)out)[i] = c->s.v8[i];
  }
}

/* calls SHA-3 once to create MAC of data or just plain SHA3
void sha3_mac (void *in, uint32_t inlen, 
  void *key, uint32_t keylen,
  void *out, uint32_t mdlen)
{
  SHA3_CTX c;
  uint32_t i;
  
  // initialize context
  SHA3_Init (&c, mdlen);
  
  // prepend the key
  SHA3_Update (&c, key, keylen);
  
  // add data
  SHA3_Update (&c, in, inlen);
  
  // finalize
  SHA3_Final (out, &c);
}*/
