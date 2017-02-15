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

#include "noekeon.h"

void Gamma(uint32_t *a)
{
  uint32_t t;
  
  a[1] ^= ~((a[3]) | (a[2]));
  a[0] ^=   a[2] & a[1];  
  
  t     = a[3]; 
  a[3]  = a[0]; 
  a[0]  = t;
  a[2] ^= a[0] ^ a[1] ^ a[3];
  
  a[1] ^= ~((a[3]) | (a[2]));
  a[0] ^=   a[2] & a[1];  
} 

void Pi1(uint32_t *a){
  a[1] = ROTL32(a[1], 1);
  a[2] = ROTL32(a[2], 5);
  a[3] = ROTL32(a[3], 2);
}

void Pi2(uint32_t *a){
  a[1] = ROTR32(a[1], 1);
  a[2] = ROTR32(a[2], 5);
  a[3] = ROTR32(a[3], 2);
}

void Theta(const uint32_t *k, uint32_t *a)
{
  uint32_t t, i;

  t = a[0] ^ a[2]; 
  t ^= ROTR32(t, 8) ^ ROTL32(t, 8);
  
  a[1] ^= t;
  a[3] ^= t;
  
  for (i=0; i<4; i++) {
    a[i] ^= k[i];
  }

  t = a[1] ^ a[3]; 
  t ^= ROTR32(t, 8) ^ ROTL32(t, 8);
  
  a[0] ^= t;
  a[2] ^= t;  

}

void Round(
    uint32_t *Key, 
    uint32_t *State, 
    uint8_t Constant1, 
    uint8_t Constant2)
{
  State[0] ^= Constant1;
  Theta(Key, State);
  State[0] ^= Constant2;
  Pi1(State);
  Gamma(State);
  Pi2(State);
}

void Noekeon(void *data, const void *k, int enc)
{
  uint8_t rc=0x80;
  int8_t i;
  w128_t nv;
  w128_t *buf, *key;
  const uint8_t rc_tab[]= 
{ 0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A, 0x2F, 
  0x5E, 0xBC, 0x63, 0xC6, 0x97, 0x35, 0x6A, 0xD4 };
  
  buf=(w128_t*)data;
  key=(w128_t*)k;
  
  if (enc==NOEKEON_ENCRYPT)
  {
    for (i=0; i<Nr; i++) {
      Round(key->w, buf->w, rc, 0);
      rc = rc_tab[i];
    }
    buf->b[0] ^= rc;
    Theta(key->w, buf->w);
  } else {
    memset(nv.b, 0, 16);

    Theta(nv.w, key->w);

    for (i=Nr-1; i>=0; --i) {
      rc = rc_tab[i];
      Round(key->w, buf->w, 0, rc);
    }
    Theta(key->w, buf->w);
    buf->b[0] ^= 0x80;
  }  
}
