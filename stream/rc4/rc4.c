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

#include "rc4.h"

/**
 * Initialize an RC4 context using key bytes
 */
void RC4_set_key (RC4_KEY *c, uint32_t len, void *key)
{
  int i, j, t;
  uint8_t *k=(uint8_t*)key;
  
  // Initialize ctx with identity sbox
  for (i=0; i<256; i++) {
    c->s[i] = (uint8_t)i;
  }

  c->x = 0;
  c->y = 0;
  
  // Randomize the sbox using key data
  for (i=0, j=0; i<256; i++) {
    j = (j + (c->s[i] + k[i % len])) % 256;
    
    t = c->s[i];
    c->s[i] = c->s[j];
    c->s[j] = t;
  }
}

/**
 * Encrypt some data using the supplied RC4 context
 * The input and output buffers may be the same buffer.
 * Since RC4 is a stream cypher, this function is used
 * for both encryption and decryption.
 */
void RC4 (RC4_KEY *c, uint32_t len, void *in, void *out)
{
  uint32_t i;
  uint8_t  x=(uint8_t)c->x, y=(uint8_t)c->y, j=0, t;
  
  for (i=0; i<len; i++) {
    // Update x and y
    x = (x + 1) % 256;
    y = (y + c->s[x]) % 256;

    // Modify sbox
    t = c->s[x];
    c->s[x] = c->s[y];
    c->s[y] = t;

    // Encrypt/decrypt next byte
    j = (c->s[x] + c->s[y]) % 256;
    ((uint8_t*)in)[i] ^= c->s[j];
  }
  c->x = x;
  c->y = y;
}

