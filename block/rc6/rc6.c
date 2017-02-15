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
  
#include "rc6.h"

void rc6_setkey (RC6_KEY *key, void *K, uint32_t keylen)
{  
  uint32_t i, j, k, A, B, L[8], *kptr=(uint32_t*)K; 
  
  // initialize L with key
  for (i=0; i<keylen/4; i++) {
    L[i] = kptr[i];
  }
  
  A=RC6_P;
  
  // initialize S with constants
  for (i=0; i<RC6_KR; i++) {
    key->x[i] = A;
    A += RC6_Q;
  }
  
  A=B=i=j=k=0;
  
  // mix with key
  for (; k < RC6_KR*3; k++)
  { 
    A = key->x[i] = ROTL32(key->x[i] + A+B, 3);  
    B = L[j]      = ROTL32(L[j] + A+B, A+B);
    
    i++;
    i %= RC6_KR;
    
    j++;
    j %= keylen/4;
  } 
}

void rc6_crypt (RC6_KEY *key, void *input, void *output, int enc)
{
  rc6_blk *in, *out;
  uint32_t A, B, C, D, T0, T1, i;
  uint32_t *k=(uint32_t*)key->x;
  
  in =(rc6_blk*)input;
  out=(rc6_blk*)output;
  
  // load plaintext/ciphertext
  A=in->v32[0];
  B=in->v32[1];
  C=in->v32[2];
  D=in->v32[3];
  
  if (enc==RC6_ENCRYPT)
  {
    B += *k; k++;
    D += *k; k++;
  } else {
    k += 43;
    C -= *k; k--;
    A -= *k; k--;
  }
  
  for (i=0; i<RC6_ROUNDS; i++)
  {
    if (enc==RC6_ENCRYPT)
    {
      T0 = ROTL32(B * (2 * B + 1), 5);
      T1 = ROTL32(D * (2 * D + 1), 5);
      
      A = ROTL32(A ^ T0, T1) + *k; k++;
      C = ROTL32(C ^ T1, T0) + *k; k++;
      // rotate 32-bits to the left
      T0 = A;
      A  = B;
      B  = C;
      C  = D;
      D  = T0;
    } else {
      T0 = ROTL32(A * (2 * A + 1), 5);
      T1 = ROTL32(C * (2 * C + 1), 5); 
      
      B  = ROTR32(B - *k, T0) ^ T1; k--;
      D  = ROTR32(D - *k, T1) ^ T0; k--;
      // rotate 32-bits to the right
      T0 = D;
      D  = C;
      C  = B;
      B  = A;
      A  = T0;
    }
  }
  
  if (enc==RC6_ENCRYPT)
  {
    A += *k; k++;
    C += *k; k++;
  } else {
    D -= *k; k--;
    B -= *k; k--;
  }
  // save plaintext/ciphertext
  out->v32[0]=A;
  out->v32[1]=B;
  out->v32[2]=C;
  out->v32[3]=D;
}
