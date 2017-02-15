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


#include <stdint.h>
#include <string.h>

#include "threefish.h"

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])

void threefish_setkey(
    threefish_ctx_t *c,
    const void *key, 
    const void *tweak)
{
  uint8_t i;
  
  memcpy((void*)c->k, key,   32);
  memcpy((void*)c->t, tweak, 16);
  
  c->k[4] = 0x1BD11BDAA9FC1A22ULL;
  
  for(i=0; i<4; i++){
    c->k[4] ^= K(i);
  }
  c->t[2] = T(0) ^ T(1);  
}

void mix(void *data, 
    uint8_t rc[], 
    int rnd, int enc)
{
  int     i;
  uint64_t *x;
  uint8_t  r;
  
  x = (uint64_t*)data;
  
  for (i=0; i<4; i += 2)
  {
    r = rc[(rnd & 7) + (i << 2)];

    if (enc==THREEFISH_DECRYPT)
    {
      x[i+1] ^= x[i];
      x[i+1]  = ROTL64(x[i+1], -(r-64));
      x[i]   -= x[i+1];      
    } else {
      x[i]   += x[i+1];
      x[i+1]  = ROTL64(x[i+1], r);
      x[i+1] ^= x[i];    
    }
  }
}

void permute(void *data)
{
  uint64_t t;
  uint64_t *x=(uint64_t*)data;
  
  t    = x[1];
  x[1] = x[3];
  x[3] = t;
}

// perform both addition and subtraction on data
// enc should be 0 for addition or 1 for subtraction
void addkey(
    const threefish_ctx_t *c, 
    void *data, uint8_t s, 
    uint64_t enc)
{
  int i;
  uint64_t x0, x1, x2;
  uint64_t *x=(uint64_t*)data;
  
  for (i=0; i<4; i++) {
    x0 = x[i];
    x1 = c->k[(s + i) % 5];
    x2 = 0;
    
    if (i==1) x2 = c->t[s % 3];
    if (i==2) x2 = c->t[(s+1) % 3];
    if (i==3) x2 = s;
    
    // add or subtract depending on enc
    x[i] = ((x0 ^ -enc) + x1 + x2) ^ -enc;
  }
}

void threefish_encrypt(
    const threefish_ctx_t *c, 
    void *data, 
    uint32_t enc)
{
  uint8_t i, s=0, ofs=1;
  uint32_t x0, x1;
  
  uint8_t rc[16] = 
  { 14, 52, 23,  5, 25, 46, 58, 32, 
    16, 57, 40, 37, 33, 12, 22, 32};

  if (enc == THREEFISH_DECRYPT) {
    s   = 18;
    ofs = ~0;
    
    // swap rotation constants if
    // decrypting
    for (i=0; i<4; i += 2) {
      x0 = ((uint32_t*)rc)[i+0];
      x1 = ((uint32_t*)rc)[i+1];

      ((uint32_t*)rc)[i+0] = SWAP32(x1);
      ((uint32_t*)rc)[i+1] = SWAP32(x0);
    }
  }
  // apply 72 rounds
  for (i=0; i<72; i++)
  {
    // add key every 4 rounds
    if((i & 3) == 0) {
      addkey(c, data, s, enc);
      s += ofs;
    }

    // permute if decrypting
    if (enc == THREEFISH_DECRYPT) {
      permute(data);
    }
    
    // apply mixing function
    mix (data, rc, i, enc);
    
    // permute if encrypting
    if (enc == THREEFISH_ENCRYPT) {
      permute(data);
    }
  }
  // add key
  addkey(c, data, s, enc);
}

