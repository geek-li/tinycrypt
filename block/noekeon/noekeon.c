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
  
#include "noekeon.h"

void Theta(
    uint32_t *a, 
    const uint32_t *k)
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
    uint32_t *s, 
    uint32_t *Key,
    int enc, 
    int rnd, 
    int end)
{
  uint32_t t;
  uint32_t rc1, rc2;
  const uint8_t rc_tab[]=   
{ 0x80,
  0x1B, 0x36, 0x6C, 0xD8, 
  0xAB, 0x4D, 0x9A, 0x2F, 
  0x5E, 0xBC, 0x63, 0xC6, 
  0x97, 0x35, 0x6A, 0xD4 };
  
  rc1 = rc_tab[rnd];
  rc2 = 0;
  if (enc==NOEKEON_DECRYPT) {
    XCHG(rc1, rc2, t);
  }
  
  s[0] ^= rc1;
  Theta(s, Key);
  s[0] ^= rc2;
  
  if (end) return;
  
  //Pi1
  s[1] = ROTL32(s[1], 1);
  s[2] = ROTL32(s[2], 5);
  s[3] = ROTL32(s[3], 2);
  
  // Gamma
  s[1] ^= ~((s[3]) | (s[2]));
  s[0] ^=   s[2] & s[1];  
  
  XCHG(s[0], s[3], t);
  
  s[2] ^= s[0] ^ s[1] ^ s[3];
  
  s[1] ^= ~((s[3]) | (s[2]));
  s[0] ^=   s[2] & s[1];  
  
  // Pi2
  s[1] = ROTR32(s[1], 1);
  s[2] = ROTR32(s[2], 5);
  s[3] = ROTR32(s[3], 2);
}

void swapcpy(
    void *dst, 
    const void *src)
{
  int i;
  for (i=0; i<4; i++) {
    ((uint32_t*)dst)[i] = SWAP32(((uint32_t*)src)[i]);
  }
}

void Noekeon(
    const void *k, 
    void *buf, 
    int enc)
{
  int i;

  uint32_t NullVector[4], State[4], Key[4];
  
  swapcpy(Key, k);
  swapcpy(State, buf);

  if (enc==NOEKEON_ENCRYPT)
  {
    for (i=0; i<=Nr; i++) {
      Round(State, Key, enc, i, i==Nr);
    }
  } else {
    memset(NullVector, 0, 16);
    Theta(Key, NullVector);

    for (i=Nr; i>=0; --i) {
      Round(State, Key, enc, i, i==0);
    }
  }
  swapcpy(buf, State);
}
