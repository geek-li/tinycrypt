/**
  Copyright © 2016 Odzhan. All Rights Reserved.

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
  
#include "s20.h"

// setup the key, must be 256-bits with 64-bit nonce
void s20_setkey(s20_ctx *c, void *key, void *nonce)
{
    s20_blk *iv=(s20_blk*)nonce;
    s20_blk *k=(s20_blk*)key;
    
    c->s.v32[ 0] = 0x61707865;
    c->s.v32[ 1] = k->v32[0];
    c->s.v32[ 2] = k->v32[1];
    c->s.v32[ 3] = k->v32[2];
    c->s.v32[ 4] = k->v32[3];
    c->s.v32[ 5] = 0x3320646E;
    c->s.v32[ 6] = iv->v32[0];
    c->s.v32[ 7] = iv->v32[1];
    c->s.v32[ 8] = 0;
    c->s.v32[ 9] = 0;
    c->s.v32[10] = 0x79622D32;
    c->s.v32[11] = k->v32[4];
    c->s.v32[12] = k->v32[5];
    c->s.v32[13] = k->v32[6];
    c->s.v32[14] = k->v32[7];
    c->s.v32[15] = 0x6B206574;
}

// transforms block using ARX instructions
void QUARTERROUND(s20_blk *blk, uint16_t idx) 
{
    uint32_t a, b, c, d;
    uint32_t *x=(uint32_t*)&blk->v8;
    
    a = (idx         & 0xF);
    b = ((idx >>  4) & 0xF);
    c = ((idx >>  8) & 0xF);
    d = ((idx >> 12) & 0xF);

    x[b] ^= ROTL32((x[a] + x[d]), 7);
    x[c] ^= ROTL32((x[b] + x[a]), 9);
    
    x[d] ^= ROTL32((x[c] + x[b]),13);
    x[a] ^= ROTL32((x[d] + x[c]),18);
}

// encrypt/decrypt 64 byte block
// do not call directly, use s20_encrypt instead
void s20_core (s20_ctx *c, uint8_t *in, uint32_t len)
{
    s20_blk x;
    int     i, j;

    // 16-bit integers of each index
    uint16_t idx16[8]=
    { 0xC840, 0x1D95, 0x62EA, 0xB73F, 
      0x3210, 0x4765, 0x98BA, 0xEDCF };
    
    // copy state to local space
    for (i=0; i<16; i++) { 
      x.v32[i] = c->s.v32[i];
    }
    // apply 20 rounds
    for (i=0; i<20; i+=2) {
      for (j=0; j<8; j++) {
        QUARTERROUND(&x, idx16[j]);
      }
    }
    // add state to x
    for (i=0; i<16; i++) {
      x.v32[i] += c->s.v32[i];
    }
    // xor input with x
    for (i=0; i<len; i++) {
      ((uint8_t*)in)[i] ^= x.v8[i];
    }
    // update block counter
    c->s.v64[4]++;
    // stopping at 2^70 bytes per nonce is user's responsibility
}

// encrypt stream of bytes
void s20_crypt (s20_ctx *c, void *in, uint32_t len) 
{
    uint32_t r;
    uint8_t  *p=(uint8_t*)in;
    
    while (len) {
      r=(len>64) ? 64 : len;
      s20_core(c, p, r);
      len -= r;
      p += r;
    }
}
