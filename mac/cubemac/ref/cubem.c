
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

#include "cm.h"

// permutation function
void permute(cube_state *s)
{
    int      i, j, k, n;
    uint32_t y[16];

    for (n=16; n>0; n--) 
    {
      for (k=7, j=2; j>0; k+=4, j--)
      {
        for (i=15; i>=0; --i) s->w[i + 16] += s->w[i];
        for (i=15; i>=0; --i) y[i ^ (j*4)] = s->w[i];
        for (i=15; i>=0; --i) s->w[i] = ROTL32(y[i], k);
      
        for (i=15; i>=0; --i) s->w[i] ^= s->w[i + 16];
        for (i=15; i>=0; --i) y[i ^ j] = s->w[i + 16];
        for (i=15; i>=0; --i) s->w[i + 16] = y[i];
      }
    }
}

// absorb data into the state
uint32_t absorb(cube_state *s, 
    const void *msg, uint32_t len)
{
    uint32_t i, idx=0;
    uint8_t  *p=(uint8_t*)msg;
    
    for (i=0; i<len; i++) {
      s->b[idx++] ^= p[i];
      if (idx == 32) {
        permute(s);
        idx = 0;
      }
    }  
    return idx;
}

// cube message authentication code
void cube_macx(const void *mkey, uint32_t keylen,
    const void *msg, uint32_t msglen, void *tag)
{
    uint32_t   idx;  
    cube_state s;
    
    // 1. initialize state
    memset(&s, 0, sizeof(cube_state));

    s.w[0] = 16; // 16-byte output
    s.w[1] = 32; // 32-byte block size
    s.w[2] = 16; // 16 rounds per block
    
    permute(&s);
    
    // 2. absorb key
    absorb (&s, mkey, 64);
    
    // 3. absorb message
    idx = absorb(&s, msg, msglen);

    // 4. absorb end bit
    s.b[idx] ^= 0x80;
    permute(&s);
    
    // 5. absorb final bit
    s.w[31] ^= 1;
    
    permute(&s);
    permute(&s);
    
    // 6. return 128-bit tag
    memcpy(tag, s.b, 16);  
}
