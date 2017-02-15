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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cc20.h"

#define memset(x,y,z) __stosb(x,y,z)
#define memcpy(x,y,z) __movsb(x,y,z)

void add(uint32_t h[17], const uint32_t c[17])
{
  uint32_t i, x = 0;
  
  for (i=0; i<17; i++) { 
    x += h[i] + c[i]; 
    h[i] = x & 255; 
    x >>= 8; 
  }
}

void freeze(uint32_t x[17])
{
  uint32_t horig[17], minusp[17];
  uint32_t i, negative;
  
  memset(minusp, 0, 17*4);

  minusp[ 0] = 5;
  minusp[16] = 252;

  memcpy ((uint8_t*)horig, (uint8_t*)x, 17*4);
  
  add(x, minusp);
  negative = -(x[16] >> 7);
  
  for (i=0; i<17; i++) {
    x[i] ^= negative & (horig[i] ^ x[i]);
  }
}

void squeeze(uint32_t x[17])
{
  uint32_t i;
  uint32_t u = 0;
  
  for (i=0; i<16; i++) { 
    u += x[i];
    x[i] = u & 255; 
    u >>= 8; 
  }
  
  u += x[16];
  x[16] = u & 3;
  u = 5 * (u >> 2);
  
  for (i=0; i<16; i++) { 
    u += x[i];
    x[i] = u & 255;
    u >>= 8;
  }
  x[16] += u;
}

// "P" is 2^130-5 or 3fffffffffffffffffffffffffffffffb
void mulmod(uint32_t h[17],const uint32_t r[17])
{
  uint32_t hr[17];
  uint32_t i, j, u;

  for (i=0; i<17; i++) {
    u = 0;
    for (j=0; j<=i; j++) {
      u += h[j] * r[i - j];
    }
    for (; j<17; j++) {
      u += 320 * h[j] * r[i + (17 - j)];
    }
    hr[i] = u;
  }
  memcpy ((uint8_t*)h, (uint8_t*)hr, 17*4);
  squeeze(h);
}

/**********************************************
 *
 * poly1305 mac function
 *
 **********************************************/
void poly1305_mac (uint8_t *out, const uint8_t *in, 
    uint32_t inlen, const uint8_t *k)
{
  uint32_t i, len;
  uint32_t r[17], acc[17], block[17];
  
  // copy r
  for (i=0; i<16; i++) {
    r[i] = k[i];
  }
  // clamp r
  r[ 3] &= 15;
  r[ 4] &= 252;
  r[ 7] &= 15;
  r[ 8] &= 252;
  r[11] &= 15;
  r[12] &= 252;
  r[15] &= 15;
  r[16]  = 0;

  // zero initialize accumulator
  memset ((uint8_t*)acc, 0, 17*4);

  while (inlen > 0) {
    // zero block
    memset ((uint8_t*)block, 0, 17*4);
    
    // process 16 bytes or remaining
    len = inlen < 16 ? inlen : 16;
    
    // store data in block
    for (i=0; i<len; i++) {
      block[i] = in[i];
    }
    
    // store 1 at end of block
    block[len] = 1;
    
    // add block to accumulator
    add (acc, block);
    
    // multiply accumulator by r mod p
    mulmod (acc, r);
    
    // update position
    in    += len;
    inlen -= len;
  }

  freeze(acc);

  // add s
  for (i=0; i<16; i++) {
    block[i] = k[i + 16];
  }
  
  block[16] = 0;
  add (acc, block);
  
  // return tag
  for (i=0; i<16; i++) {
    out[i] = acc[i];
  }
}


