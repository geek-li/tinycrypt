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
  
#include "tea.h"

void TEA_Encrypt (void *data, void *key, int enc)
{
  tea_blk *v=(tea_blk*)data;
  tea_key *k=(tea_key*)key;
  
  uint32_t v0, v1, i, sum=0x9e3779b9;
  
  if (enc==TEA_DECRYPT) {
    sum <<= 5;
  }
  
  v0 = (v->d[0]); 
  v1 = (v->d[1]);
  
  if (enc==TEA_ENCRYPT)
  {
    for (i=0; i<TEA_ROUNDS; i++) 
    {         
      v0 += ((v1 << 4) + k->d[0]) ^ 
             (v1 + sum) ^ 
            ((v1 >> 5) + k->d[1]);
            
      v1 += ((v0 << 4) + k->d[2]) ^ 
             (v0 + sum) ^ 
            ((v0 >> 5) + k->d[3]);
            
      sum += 0x9e3779b9;
    }
  } else {
    for (i=0; i<TEA_ROUNDS; i++) 
    {
      v1 -= ((v0 << 4) + k->d[2]) ^ 
             (v0 + sum) ^ 
            ((v0 >> 5) + k->d[3]);
              
      v0 -= ((v1 << 4) + k->d[0]) ^ 
             (v1 + sum) ^ 
            ((v1 >> 5) + k->d[1]);
            
      sum -= 0x9e3779b9;
    }  
  }
  v->d[0] = (v0); 
  v->d[1] = (v1);
}

