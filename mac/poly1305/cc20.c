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
  
#include "cc20.h"

/**********************************************
 *
 * initialize state with key, nonce and counter
 *
 **********************************************/
void cc20_init(cc20_state *s, const void *key, 
    const uint32_t counter, const void *nonce)
{
    uint32_t *n=(uint32_t*)nonce;
    int      i;
    
    // "expand 32-byte k"
    s->w[0] = 0x61707865;
    s->w[1] = 0x3320646E;
    s->w[2] = 0x79622D32;
    s->w[3] = 0x6B206574;

    // copy 256-bit key
    memcpy (&s->b[16], key, 32);
    
    // set 96-bit nonce and 32-bit counter
    s->w[12] = counter;
    s->w[13] = n[0];
    s->w[14] = n[1];
    s->w[15] = n[2];
}

/**********************************************
 *
 * permute state
 *
 **********************************************/
void cc20_permute(cc20_state *state) 
{
    uint32_t a, b, c, d, i, j, idx;
    uint32_t *x = &state->w[0];
    
    // 16-bit integers of each index
    uint16_t idx16[8]=
    { 0xC840, 0xD951, 0xEA62, 0xFB73, 
      0xFA50, 0xCB61, 0xD872, 0xE943 };
      
    // inner_block(working_state)
    for (i=0; i<20; i+=2) 
    {
      for (j=0; j<8; j++)
      {
        idx  = idx16[j];
        
        a    = (idx         & 0xF);
        b    = ((idx >>  4) & 0xF);
        c    = ((idx >>  8) & 0xF);
        d    = ((idx >> 12) & 0xF);
        
        x[a] = x[a] + x[b]; 
        x[d] = ROTL32(x[d] ^ x[a],16);
        
        x[c] = x[c] + x[d]; 
        x[b] = ROTL32(x[b] ^ x[c],12);
        
        x[a] = x[a] + x[b]; 
        x[d] = ROTL32(x[d] ^ x[a], 8);
        
        x[c] = x[c] + x[d]; 
        x[b] = ROTL32(x[b] ^ x[c], 7);
      }
    }
}
    
/**********************************************
 *
 * generate stream from existing state
 *
 **********************************************/
void cc20_stream (
    cc20_state *s, 
    const void *key, 
    const uint32_t counter, 
    const void *nonce)
{
    cc20_state ws;
    int        i;
    
    cc20_init(s, key, counter, nonce);
    
    // working_state = state
    memcpy ((uint8_t*)&ws, s->b, CC20_STATE_LEN);
    
    // update state
    cc20_permute(&ws);
    
    // state += working_state
    for (i=0; i<16; i++) {
      s->w[i] += ws.w[i];
    }
}

/**********************************************
 *
 * encrypt arbitrary length of data
 *
 **********************************************/
void cc20_encrypt (const void *key, uint32_t counter, 
    const void *nonce, void *data, uint32_t len) 
{
    uint32_t    i, j, r;
    cc20_state  s;
    uint8_t     *ptr=(uint8_t*)data;
    
    for (i=0; len != 0; i++) 
    {
      cc20_stream(&s, key, counter + i, nonce);
      r = (len > CC20_STATE_LEN) ? CC20_STATE_LEN : len;
      // encrypt plaintext
      for (j=0; j<r; j++) {
        ptr[j] ^= s.b[j];
      }
      len -= r;
      ptr += r;
    }
}

/**********************************************
 *
 * poly1305 key generation
 *
 **********************************************/
void poly1305_key_gen(
    void *out, 
    const void *key, 
    const void *nonce)
{
  cc20_state s;
  uint32_t   ctr = 0;
  
  cc20_stream(&s, key, ctr, nonce);
  memcpy (out, s.b, 32);
}


