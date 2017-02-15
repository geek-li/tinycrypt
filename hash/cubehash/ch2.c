
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

#include "parameters.h"
#include "cubehash.h"

// i and r parameters are 10 which is default
// configuration

#define CH_R_PARAM 16   // r parameter
#define CH_B_PARAM 1   // b parameter
#define CH_H_PARAM 64  // h parameter in bits

#define ROTL32(a,b) (((a) << (b)) | ((a) >> (32 - b)))

void transform(cube_ctx *c, int cnt)
{
  int      i, j, k, n;
  uint32_t y[16];

  for (n=cnt; n>0; n--) 
  {
    for (k=7, j=2; j>0; k+=4, j--)
    {
      for (i=15; i>=0; --i) c->x[i + 16] += c->x[i];
      for (i=15; i>=0; --i) y[i ^ (j*4)] = c->x[i];
      for (i=15; i>=0; --i) c->x[i] = ROTL32(y[i], k);
    
      for (i=15; i>=0; --i) c->x[i] ^= c->x[i + 16];
      for (i=15; i>=0; --i) y[i ^ j] = c->x[i + 16];
      for (i=15; i>=0; --i) c->x[i + 16] = y[i];
    }
  }
}

void cube_init(cube_ctx *c)
{
  int i;

  // initialize state
  for (i=0; i<32; ++i) {
    c->x[i] = 0;
  }
  
  // set parameters
  c->x[0] = CH_H_PARAM/8; // h-bit in bytes
  c->x[1] = CH_B_PARAM;   // b (block size in bytes)
  c->x[2] = CH_R_PARAM;   // r (number of rounds for each block)
  c->idx  = 0;
  
  transform(c, CH_R_PARAM*10); // initial 10 rounds
}

void cube_update(cube_ctx *c, const void *in, uint32_t len)
{
  uint32_t i;
  
  if (len==0) return;
  
  for (i=0; i<len; i++) {
    c->v8[c->idx++] ^= ((uint8_t*)in)[i];
    if (c->idx==CH_B_PARAM) {
      transform(c, CH_R_PARAM);
      c->idx=0;
    }
  }
}

void cube_final(void *out, cube_ctx *c)
{
  int      i;
  uint32_t u;
  
  // 1. absorb end bit, apply rounds
  c->v8[c->idx] ^= 0x80;
  transform(c, CH_R_PARAM);
  
  // 2. absorb final bit, apply final rounds
  c->x[31] ^= 1;
  transform(c, 10*CH_R_PARAM);
  
  // 3. return h-bit hash
  for (i=0; i<CH_H_PARAM/8; i++) 
    ((uint8_t*)out)[i] = c->v8[i];
}
