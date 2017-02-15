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
  POSSIBILITY OF SUCH DAMAGE.*/

#include "b2s.h"

// G mixing function
void b2s_g(blake_blk *x, uint16_t index, 
    uint32_t x0, uint32_t x1) 
{
  uint32_t a, b, c, d;
  
  a = (index         & 0xF);
  b = ((index >>  4) & 0xF);
  c = ((index >>  8) & 0xF);
  d = ((index >> 12) & 0xF);
  
  x->v32[a] += x->v32[b] + x0; 
  x->v32[d] = ROTR32(x->v32[d] ^ x->v32[a], 16);
  
  x->v32[c] += x->v32[d]; 
  x->v32[b] = ROTR32(x->v32[b] ^ x->v32[c], 12);
  
  x->v32[a] += x->v32[b] + x1; 
  x->v32[d] = ROTR32(x->v32[d] ^ x->v32[a],  8);
  
  x->v32[c] += x->v32[d]; 
  x->v32[b] = ROTR32(x->v32[b] ^ x->v32[c],  7);
}

// F compression function
void b2s_compress (b2s_ctx *ctx, int last)
{
  uint32_t  i, j, x0, x1;
  uint64_t  x;
  blake_blk v, m;
  
  // initialization vectors
  uint32_t b2s_iv[8] =
  { 0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };
    
  // work vector indices
  uint16_t idx16[8]=
  { 0xC840, 0xD951, 0xEA62, 0xFB73, 
    0xFA50, 0xCB61, 0xD872, 0xE943 };

  // permutations
  uint64_t sigma64[10] = 
  { 0xfedcba9876543210, 0x357b20c16df984ae,
    0x491763eadf250c8b, 0x8f04a562ebcd1397,
    0xd386cb1efa427509, 0x91ef57d438b0a6c2,
    0xb8293670a4def15c, 0xa2684f05931ce7bd,
    0x5a417d2c803b9ef6, 0x0dc3e9bf5167482a };
  
  // initialize v work vector with iv and state
  for (i=0; i<BLAKE2s_LBLOCK; i++) {
    v.v32[i    ] = ctx->state.v32[i];
    v.v32[i + 8] = b2s_iv[i];
  }
  
  // copy message buffer into m
  for (i=0; i<BLAKE2s_CBLOCK/4; i++) {
    m.v32[i] = ctx->buffer.v32[i];
  }

  // xor v with current length
  v.v32[12] ^= ctx->len.v32[0];
  v.v32[13] ^= ctx->len.v32[1];
  
  // if this is last block, invert word 14
  if (last) {
    v.v32[14] = ~v.v32[14];
  }
  
  // the minimum is 10 rounds but this can be increased
  // to deal with situation 10 rounds is no longer
  // adequate for protection.
  for (i=0; i<ctx->rounds; i++) {
    x = sigma64[i%10];
    // 8 mixing
    for (j=0; j<8; j++) {
      x0 = (x & 15); x >>= 4;
      x1 = (x & 15); x >>= 4;
      b2s_g(&v, idx16[j], m.v32[x0], m.v32[x1]);
    }
  }
  // update state with v
  for (i=0; i<BLAKE2s_LBLOCK; i++) {
    ctx->state.v32[i] ^= v.v32[i] ^ v.v32[i + 8];
  }
}

// initialize context. key and rnds are optional
void b2s_init (b2s_ctx *ctx, uint32_t outlen, 
  void *key, uint32_t keylen, uint32_t rnds)
{
  uint32_t i;

  // initialization vectors
  uint32_t b2s_iv[8] =
  { 0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };
  
	// outlen can't be zero or exceed 32 bytes
	outlen=(outlen==0 || outlen>32) ? 32 : outlen;
	// keylen can't exceed 32 bytes
	keylen=(keylen>32)  ? 32 : keylen;
	
  // initialize state iv
  for (i=0; i<8; i++) {
    ctx->state.v32[i] = b2s_iv[i];
  }
  // update state with keylen and outlen
  ctx->state.v32[0] ^= 0x01010000 ^ 
                      (keylen << 8) ^ 
                      outlen;

  // set buffer to zero
  for (i=0; i<16; i++) {
    ctx->buffer.v32[i] = 0;
  }
  
  // set length to zero
  ctx->len.v64 = 0;
  // if key used, set index to 64
  ctx->index   = keylen ? 64 : 0;
  ctx->outlen  = outlen;
  // minimum rounds are 10
  // note that non-standard rounds may undermine security.
  // this feature is not part of the standard!
  ctx->rounds  = rnds < 10 ? 10 : rnds; 

  // copy optional key
  for (i=0; i<keylen; i++) {
    ctx->buffer.v8[i] = ((uint8_t*)key)[i];
  }
}

// update context
void b2s_update (b2s_ctx *ctx, 
  void *input, uint32_t len)
{
  uint32_t i;
  uint8_t *p=(uint8_t*)input;
  
  if (len==0) return;
  
  // update buffer and state
  for (i=0; i<len; i++) {
    if (ctx->index==64) {
      ctx->len.v64 += ctx->index;
      b2s_compress (ctx, 0);
      ctx->index = 0;
    }
    ctx->buffer.v8[ctx->index++] = p[i];
  }
}

// Finalize
void b2s_final (void* out, b2s_ctx *ctx)
{
  uint32_t i;
  
  // update length with current index to buffer
  ctx->len.v64 += ctx->index;
  
  // zero remainder of buffer
  while (ctx->index < 64) {
    ctx->buffer.v8[ctx->index++] = 0;
  }

  // compress last block
  b2s_compress (ctx, 1);

  // copy state to output
  for (i=0; i<ctx->outlen; i++) {
    ((uint8_t*)out)[i] = ctx->state.v8[i];
  }
}
