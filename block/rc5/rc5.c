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
  
#include "rc5.h"

void rc5_setkey (RC5_CTX *key, void *input)
{  
  uint32_t i, j, k, A, B, T, L[4], *kptr=(uint32_t*)input; 
  
  // initialize L with key
  for (i=0; i<4; i++) {
    L[i] = kptr[i];
  }
  
  A=RC5_P;
  
  // initialize S with constants
  for (i=0; i<RC5_KR; i++) {
    key->x[i] = A;
    A += RC5_Q;
  }
  
  A=B=i=j=k=0;
  
  // mix with key
  for (; k < RC5_KR*3; k++)
  { 
    A = key->x[i] = ROTL32(key->x[i] + A+B, 3);  
    B = L[j]      = ROTL32(L[j] + A+B, A+B);
    
    i++;
    i %= RC5_KR;
    
    j++;
    j %= RC5_KEYLEN/4;
  } 
}

void rc5_crypt (RC5_CTX *key, void* input, void* output, int enc)
{
  rc5_blk *in, *out;
  uint32_t *k=(uint32_t*)key->x;
  uint32_t A, B, T, i;
  
  in=input;
  out=output;
  
  A=in->v32[0];
  B=in->v32[1];
  
  if (enc==RC5_ENCRYPT) {
    A += *k; k++;
    B += *k; k++;
  } else {
    k += RC5_KR - 1;
  }
  
  for (i=0; i<RC5_KR-2; i++)
  {
    if (enc==RC5_ENCRYPT) {
      A = ROTL32(A ^ B, B) + *k; k++;
    } else {
      B = ROTR32(B - *k, A) ^ A; k--;
    }
    // swap
    T = B;
    B = A;
    A = T;
  }
  if (enc==RC5_DECRYPT) {
    B -= *k; k--;
    A -= *k; k--;
  }
  out->v32[0]=A;
  out->v32[1]=B;
}

