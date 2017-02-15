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

#include "chaskey.h"
  
void chas_setkey(void *out, void *in) 
{
  int      i;
  uint32_t *k=(uint32_t*)out;
  
  memcpy (out, in, 16);
  
  for (i=0; i<2; i++)
  {
    k[4] = (k[0] << 1); 
    
    k[4] ^= 0x87 * (k[3] >> 31);
    
    k[5] = (k[1] << 1) | (k[0] >> 31); 
    k[6] = (k[2] << 1) | (k[1] >> 31); 
    k[7] = (k[3] << 1) | (k[2] >> 31);
    
    k += 4;    
  }
}

void chas_permute(uint32_t v[])
{
  int i=12;
  
  do
  {
    v[0] += v[1]; 
    v[1]=ROTL32(v[1], 5); 
    v[1] ^= v[0]; 
    v[0]=ROTL32(v[0],16); 
    v[2] += v[3]; 
    v[3]=ROTL32(v[3], 8); 
    v[3] ^= v[2]; 
    v[0] += v[3]; 
    v[3]=ROTL32(v[3],13); 
    v[3] ^= v[0]; 
    v[2] += v[1]; 
    v[1]=ROTL32(v[1], 7); 
    v[1] ^= v[2]; 
    v[2]=ROTL32(v[2],16); 
  } while (--i);
}

void chas_xor(state *out, const void *in, int len) {
  int i;

  for (i=0; i<len; i++) {
    out->b[i] ^= ((uint8_t*)in)[i];
  }
}

void chas_mac (uint8_t *tag, 
    uint8_t *msg, uint32_t msglen, uint8_t *key) 
{
  state v;
  int   len;
  
  // copy 16 bytes of key
  memcpy(v.b, key, 16);

  // absorb message 
  for (;;)
  {
    len = (msglen < 16) ? msglen : 16;
    
    // xor state with msg data
    chas_xor(&v, msg, len);

    // final?
    if (msglen <= 16) {
      if (msglen < 16) {
        // final? add padding bit
        v.b[msglen] ^= 0x01;
      }
      key += (msglen == 16) ? 16 : 32;
      break;
    }    
    
    // apply permutation function
    chas_permute(v.w);
    
    // update position and length
    msg += 16;
    msglen -= 16;
  }

  // pre-whiten
  chas_xor(&v, key, 16);
  // permute
  chas_permute(v.w);
  // post-whiten
  chas_xor(&v, key, 16);
  // return tag
  memcpy(tag, v.b, 16);
}
