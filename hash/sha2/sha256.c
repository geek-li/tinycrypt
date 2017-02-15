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

#include "sha2.h"

#ifdef DYNAMIC

#include <math.h>
#pragma intrinsic(fabs,pow,sqrt)

uint16_t p[64] =
{  2,   3,   5,   7,  11,  13,  17,  19, 
  23,  29,  31,  37,  41,  43,  47,  53,
  59,  61,  67,  71,  73,  79,  83,  89, 
  97, 101, 103, 107, 109, 113, 127, 131,
 137, 139, 149, 151, 157, 163, 167, 173, 
 179, 181, 191, 193, 197, 199, 211, 223,
 227, 229, 233, 239, 241, 251, 257, 263, 
 269, 271, 277, 281, 283, 293, 307, 311 };

// square root of integer, return fractional part as integer
uint32_t sqrt2int (uint32_t x) {
    uint32_t r;
    r = (uint32_t)(fabs(sqrt((double)p[x]))*pow(2,32));
    return r;
}

// cube root of integer, return fractional part as integer
uint32_t cbr2int (uint32_t x) {
    uint32_t r;
    r = (uint32_t)(fabs(pow((double)p[x],1.0/3.0))*pow(2,32));
    return r;
}

#endif

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR32(x,2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define EP1(x) (ROTR32(x,6) ^ ROTR32(x,11) ^ ROTR32(x,25))
#define SIG0(x) (ROTR32(x,7) ^ ROTR32(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTR32(x,17) ^ ROTR32(x,19) ^ ((x) >> 10))

/************************************************
*
* update state with block of data
*
************************************************/
void SHA256_Transform (SHA256_CTX *c) 
{
    uint32_t t1, t2, i, j, t;
    uint32_t w[64], s[8];
#ifndef DYNAMIC
    uint32_t k[64]=
    { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
      0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 
      0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
      0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 
      0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 
      0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
#endif  
    // load data in big endian format
    for (i=0; i<16; i++) {
      w[i] = SWAP32(c->buf.v32[i]);
    }

    // expand data into 512-bit buffer
    for (i=16; i<64; i++) {
      w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    }
    
    // load state into local buffer
    for (i=0; i<8; i++) {
      s[i] = c->s.v32[i];
    }
    
    // for 64 rounds
    for (i=0; i<64; i++)
    {
      t1 = s[7] + EP1(s[4]) + CH(s[4], s[5], s[6]) + w[i];
      #ifdef DYNAMIC
      t1 += cbr2int (i);
      #else
      t1 += k[i];
      #endif
      t2 = EP0(s[0]) + MAJ(s[0], s[1], s[2]);
      s[7]  = t1 + t2;
      s[3] += t1;
      // rotate "right" 32-bits
      t1=s[0]; // load a
      for (j=1; j<8; j++) {
        t2=s[j];
        s[j]=t1;
        t1=t2;
      }
      s[0]=t1;
    }
    
    // save state
    for (i=0; i<8; i++) {
      c->s.v32[i] += s[i];
    }
}

/************************************************
*
* initialize context
*
************************************************/
void SHA256_Init (SHA256_CTX *c) {
    
    #ifdef DYNAMIC
    int i;
    for (i=0; i<SHA256_LBLOCK; i++) {
      c->s.v32[i] = sqrt2int(i);
    }
    #else
    c->s.v32[0] = 0x6a09e667;
    c->s.v32[1] = 0xbb67ae85;
    c->s.v32[2] = 0x3c6ef372;
    c->s.v32[3] = 0xa54ff53a;
    c->s.v32[4] = 0x510e527f;
    c->s.v32[5] = 0x9b05688c;
    c->s.v32[6] = 0x1f83d9ab;
    c->s.v32[7] = 0x5be0cd19;
    #endif
    c->len = 0;
}

/************************************************
*
* update state with input
*
************************************************/
void SHA256_Update (SHA256_CTX *c, void *in, size_t len) {
    uint8_t *p = (uint8_t*)in;
    size_t r, idx;
    
    // get buffer index
    idx = c->len & SHA256_CBLOCK - 1;
    
    // update length
    c->len += len;
    
    while (len) {
      r = MIN(len, SHA256_CBLOCK - idx);
      memcpy (&c->buf.v8[idx], p, r);
      if ((idx + r) < SHA256_CBLOCK) break;
      
      SHA256_Transform (c);
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
void SHA256_Final (void* dgst, SHA256_CTX *c)
{
    int i;
    
    // see what length we have ere..
    uint64_t len=c->len & SHA256_CBLOCK - 1;
    // fill remaining with zeros
    memset (&c->buf.v8[len], 0, SHA256_CBLOCK - len);
    // add the end bit
    c->buf.v8[len] = 0x80;
    // if exceeding 56 bytes, transform it
    if (len >= 56) {
      SHA256_Transform (c);
      // clear buffer
      memset (c->buf.v8, 0, SHA256_CBLOCK);
    }
    // add total bits
    c->buf.v64[7] = SWAP64(c->len * 8);
    // compress
    SHA256_Transform(c);
    
    // swap byte order
    for (i=0; i<SHA256_LBLOCK; i++) {
      c->s.v32[i] = SWAP32(c->s.v32[i]);
    }
    // copy digest to buffer
    memcpy (dgst, c->s.v8, SHA256_DIGEST_LENGTH);
}
