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
  
#include "cc20.h"

// setup the key
void cc20_setkey(cc20_ctx *c, void *key, void *nonce)
{
    cc20_blk *iv=(cc20_blk*)nonce;
    int      i;
    
    // "expand 32-byte k"
    c->s.v32[0] = 0x61707865;
    c->s.v32[1] = 0x3320646E;
    c->s.v32[2] = 0x79622D32;
    c->s.v32[3] = 0x6B206574;

    // copy 256-bit key
    for (i=0; i<32; i++) {
      c->s.v8[16+i] = ((uint8_t*)key)[i];
    }
    
    // set 64-bit block counter and 64-bit nonce/iv
    c->s.v32[12] = 0;
    c->s.v32[13] = 0;
    c->s.v32[14] = iv->v32[0];
    c->s.v32[15] = iv->v32[1];
}

// transforms block using ARX instructions
void QUARTERROUND(cc20_blk *blk, uint16_t idx) 
{
    uint32_t a, b, c, d;
    uint32_t *x=(uint32_t*)&blk->v8;
    
    a = (idx         & 0xF);
    b = ((idx >>  4) & 0xF);
    c = ((idx >>  8) & 0xF);
    d = ((idx >> 12) & 0xF);
    
    x[a] = x[a] + x[b]; 
    x[d] = ROTL32(x[d] ^ x[a],16);
    
    x[c] = x[c] + x[d]; 
    x[b] = ROTL32(x[b] ^ x[c],12);
    
    x[a] = x[a] + x[b]; 
    x[d] = ROTL32(x[d] ^ x[a], 8);
    
    x[c] = x[c] + x[d]; 
    x[b] = ROTL32(x[b] ^ x[c], 7);
}

// encrypt/decrypt 64 byte block
// do not call directly, use cc20_encrypt instead
void cc20_core (cc20_ctx *c, uint8_t *in, uint32_t len)
{
    cc20_blk x;
    int      i, j;

    // 16-bit integers of each index
    uint16_t idx16[8]=
    { 0xC840, 0xD951, 0xEA62, 0xFB73, 
      0xFA50, 0xCB61, 0xD872, 0xE943 };
    
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
    c->s.v64[6]++;
    // stopping at 2^70 bytes per nonce is user's responsibility
}

// encrypt stream of bytes
void cc20_encrypt (cc20_ctx *ctx, void *in, uint32_t len) 
{
    uint32_t r;
    uint8_t  *p=(uint8_t*)in;
    
    while (len) {
      r=(len>64) ? 64 : len;
      cc20_core(ctx, p, r);
      len -= r;
      p += r;
    }
}
