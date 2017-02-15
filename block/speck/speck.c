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

#include "speck.h"

void speck_setkey(const void *in, void *ks)
{
  uint32_t i, t, k0, k1, k2, k3;

  // copy 128-bit key to local space
  k0 = ((uint32_t*)in)[0];
  k1 = ((uint32_t*)in)[1];
  k2 = ((uint32_t*)in)[2];
  k3 = ((uint32_t*)in)[3];

  // expand 128-bit key into round keys
  for (i=0; i<SPECK_RNDS-1; i++)
  {
    ((uint32_t*)ks)[i] = k0;
    
    k1 = (ROTR32(k1, 8) + k0) ^ i;
    k0 = ROTL32(k0, 3) ^ k1;
    
    // rotate left 32-bits
    XCHG(k3, k2, t);
    XCHG(k3, k1, t);
  }
}

void speck_encrypt(const void *keys, int enc, void *in)
{
  uint8_t i;
  uint32_t *ks=(uint32_t*)keys;
  
  // copy input to local space
  uint32_t x0=((uint32_t*)in)[0];
  uint32_t x1=((uint32_t*)in)[1];
  
  for (i=0; i<SPECK_RNDS; i++)
  {
    if (enc==SPECK_DECRYPT)
    {
      x1 = ROTR32(x1 ^ x0, 3);
      x0 = ROTL32((x0 ^ ks[SPECK_RNDS-1-i]) - x1, 8);        
    } else {
      x0 = (ROTR32(x0, 8) + x1) ^ ks[i];
      x1 = ROTL32(x1, 3) ^ x0;
    }
  }
  // save result
  ((uint32_t*)in)[0] = x0;
  ((uint32_t*)in)[1] = x1;
}

