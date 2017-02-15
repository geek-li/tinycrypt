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

#define memset(x,y,z) __stosb(x,y,z)
#define memcpy(x,y,z) __movsb(x,y,z)

/**********************************************
 *
 * poly1305 addition
 *
 **********************************************/
void poly1305_add(
    uint32_t out[], 
    const uint8_t in[], 
    int inlen, 
    uint8_t last)
{
  uint32_t c, i, x = 0;
  uint8_t *p = (uint8_t*)in;

  // add in to out
  for (i=0; i<17; i++) {
    c = *p;
    c = (i == inlen) ? last : c;
    c = (i  > inlen) ? 0    : c;
    p = (i  < inlen) ? p+1  : p;
    
    x += out[i] + c; 
    out[i] = x & 255; 
    x >>= 8; 
  }
}

/**********************************************
 *
 * poly1305 modular multiplication
 *
 * "P" is 2^130-5 or 3fffffffffffffffffffffffffffffffb
 *
 **********************************************/
void poly1305_mulmod(
    uint32_t acc[17],
    const uint32_t r[17])
{
  uint32_t hr[17];
  uint32_t i, j, u;

  memcpy ((uint8_t*)hr, (uint8_t*)acc, 17*4);
  
  // multiply
  for (i=0; i<17; i++) {
    u = 0;
    for (j=0; j<=i; j++) {
      u += hr[j] * r[i - j];
    }
    for (; j<17; j++) {
      u += hr[j] * r[i + (17 - j)] * 320;
    }
    acc[i] = u;
  }
  
  for (u=0, j=0; j<2; j++)
  {
    for (i=0; i<16; i++) 
    { 
      u += acc[i];
      acc[i] = u & 255; 
      u >>= 8; 
    }
    if (!j) 
    {
      u += acc[16];
      acc[16] = u & 3;
      u = (u >> 2) * 5;
    }
  }
  acc[16] += u;
}

/**********************************************
 *
 * poly1305 mac function
 *
 **********************************************/
void poly1305_mac (
    uint8_t *out, 
    const uint8_t *in, 
    uint32_t inlen, 
    const uint8_t *k)
{
  uint32_t i, len, neg;
  uint32_t r[17], acc[17];
  uint8_t  minusp[16]={5};
  
  // copy r
  for (i=0; i<16; i++) {
    r[i] = k[i];
  }
  // clamp r
  r[ 3] &= 15;
  r[ 7] &= 15;
  r[11] &= 15;
  r[15] &= 15;
  
  r[ 4] &= 252;
  r[ 8] &= 252;
  r[12] &= 252;
  r[16]  = 0;

  // zero initialize accumulator
  memset ((uint8_t*)acc, 0, 17*4);

  for (;;) 
  {
    // if zero length, break
    if (inlen==0) break;
    
    // process 16 bytes or remaining
    len = inlen < 16 ? inlen : 16;
    
    // add bytes to acc
    poly1305_add (acc, in, len, 1);

    // multiply accumulator by r mod p
    poly1305_mulmod (acc, r);

    // update length and buffer position
    in    += len;
    inlen -= len;
  }

  memcpy (r, acc, sizeof(r));

  poly1305_add (acc, minusp, 16, 252);
  neg = -(acc[16] >> 7);
  
  for (i=0; i<17; i++) {
    acc[i] ^= neg & (r[i] ^ acc[i]);
  }
  
  // add s
  poly1305_add (acc, &k[16], 16, 0);
  
  // return tag
  for (i=0; i<16; i++) {
    out[i] = acc[i];
  }
}


