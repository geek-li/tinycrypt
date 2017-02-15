
/**
  Copyright © 2017 Odzhan. All Rights Reserved.

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

#include "ch.h"

void permute(cube_ctx *c)
{
  int      i, j, k, n;
  uint32_t y[16];

  for (n=16; n>0; n--) 
  {
    for (k=7, j=2; j>0; k+=4, j--)
    {
      for (i=15; i>=0; --i) c->w[i + 16] += c->w[i];
      for (i=15; i>=0; --i) y[i ^ (j*4)] = c->w[i];
      for (i=15; i>=0; --i) c->w[i] = ROTL32(y[i], k);
    
      for (i=15; i>=0; --i) c->w[i] ^= c->w[i + 16];
      for (i=15; i>=0; --i) y[i ^ j] = c->w[i + 16];
      for (i=15; i>=0; --i) c->w[i + 16] = y[i];
    }
  }
}

void cube_setkey(cube_ctx *c, void *mkey)
{
  int i, j;
  uint32_t *key=(uint32_t*)mkey;
  
  memset(c, 0, sizeof(cube_ctx));

  c->w[0] = 16; // 128-bits in bytes
  c->w[1] = 32; // b (block size in bytes)
  c->w[2] = 16; // r (number of rounds for each block)
  
  permute(c); // initial rounds
  
  // absorb 512-bits of key
  for (i=0; i<2; i++) 
  {
    // absorb 256-bits of key
    for (j=0; j<8; j++) {
      c->w[j] ^= *key++;
    }
    permute(c); // initial rounds  
  }
}

void cube_mac(cube_ctx *c, const void *in, 
    uint32_t len, void *out)
{
  uint32_t i, idx=0;
  
  // 1. absorb message
  for (i=0; i<len; i++) {
    c->b[idx++] ^= ((uint8_t*)in)[i];
    if (idx == 32) {
      permute(c);
      idx=0;
    }
  }
  // 2. absorb end bit
  c->b[idx] ^= 0x80;
  permute(c);
  
  // 3. absorb final bit, apply final 32 rounds
  c->w[31] ^= 1;
  
  for (i=0; i<2; i++) {
    permute(c);
  }
  
  // 4. return 128-bit hash
  memcpy(out, c->b, 16);  
}
