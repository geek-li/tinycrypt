/**
  Copyright © 2015 Odzhan, Peter Ferrie. All Rights Reserved.

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

#include "b2b.h"

// initialization vectors
uint64_t b2b_iv[8] =
{ 0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
  0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
  0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
  0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179 };

// work vector indices
uint16_t idx16[8]=
{ 0xC840, 0xD951, 0xEA62, 0xFB73, 
  0xFA50, 0xCB61, 0xD872, 0xE943 };

// permutations
uint64_t sigma64[10] = 
{ 0xFEDCBA9876543210, 0x357B20C16DF984AE,
  0x491763EADF250C8B, 0x8F04A562EBCD1397,
  0xD386CB1EFA427509, 0x91EF57D438B0A6C2,
  0xB8293670A4DEF15C, 0xA2684F05931CE7BD,
  0x5A417D2C803B9EF6, 0x0DC3E9BF5167482A };

// G mixing function
void b2b_g(blake_blk *x, uint16_t index, 
    uint64_t x0, uint64_t x1) 
{
  uint64_t a, b, c, d;
  
  a = (index         & 0xF);
  b = ((index >>  4) & 0xF);
  c = ((index >>  8) & 0xF);
  d = ((index >> 12) & 0xF);
  
  x->v64[a] += x->v64[b] + x0; 
  x->v64[d] = ROTR64(x->v64[d] ^ x->v64[a], 32);
  
  x->v64[c] += x->v64[d]; 
  x->v64[b] = ROTR64(x->v64[b] ^ x->v64[c], 24);
  
  x->v64[a] += x->v64[b] + x1; 
  x->v64[d] = ROTR64(x->v64[d] ^ x->v64[a], 16);
  
  x->v64[c] += x->v64[d]; 
  x->v64[b] = ROTR64(x->v64[b] ^ x->v64[c], 63);
}

// F compression function
void b2b_compress (b2b_ctx *ctx, int last)
{
  uint64_t  i, j, x0, x1;
  uint64_t  x;
  blake_blk v, m;
  
  // initialize v work vector with iv and state
  for (i=0; i<8; i++) {
    v.v64[i    ] = ctx->state.v64[i];
    v.v64[i + 8] = b2b_iv[i];
  }
  
  // copy message buffer into m
  for (i=0; i<128/8; i++) {
    m.v64[i] = ctx->buffer.v64[i];
  }

  // xor v with current length
  v.v64[12] ^= ctx->len.v64;
  
  // if this is last block, invert word 14
  if (last) {
    v.v64[14] = ~v.v64[14];
  }
  
  // the minimum is 12 rounds but this can be increased
  // to deal with situation 12 rounds is no longer
  // adequate for protection. however, it should
  // be noted that this isn't part of the standard
  // and may undermine security of the function.
  for (i=0; i<ctx->rounds; i++) {
    x = sigma64[i%10];
    // 8 mixing
    for (j=0; j<8; j++) {
      x0 = (x & 15); x >>= 4;
      x1 = (x & 15); x >>= 4;
      b2b_g(&v, idx16[j], m.v64[x0], m.v64[x1]);
    }
  }
  // update state with v
  for (i=0; i<BLAKE2b_LBLOCK; i++) {
    ctx->state.v64[i] ^= v.v64[i] ^ v.v64[i + 8];
  }
}

// initialize context. key and rnds are optional
void b2b_init (b2b_ctx *ctx, uint32_t outlen, 
  void *key, uint32_t keylen, uint32_t rnds)
{
  uint32_t i;

	// outlen can't exceed 64 bytes
	outlen=(outlen!=64) ? 64 : outlen;
	// keylen can't exceed 64 bytes
	keylen=(keylen>64)  ? 64 : keylen;

  // initialize state iv
  for (i=0; i<8; i++) {
    ctx->state.v64[i] = b2b_iv[i];
  }
  // update state with keylen and outlen
  ctx->state.v64[0] = 0x6A09E667F3BCC908 ^ 
                      0x01010000         ^ 
                      (keylen << 8)      ^ 
                      outlen;

  // set buffer to zero
  for (i=0; i<BLAKE2b_CBLOCK/8; i++) {
    ctx->buffer.v64[i] = 0;
  }
  
  // set length to zero
  ctx->len.v64 = 0;
  // if key used, set index to 128
  ctx->index   = keylen ? 128 : 0;
  ctx->outlen  = outlen;
  // minimum rounds are 12
  // note that non-standard rounds may undermine security.
  // this feature is not part of the standard!
  ctx->rounds  = rnds < 12 ? 12 : rnds; 

  // copy optional key
  for (i=0; i<keylen; i++) {
    ctx->buffer.v8[i] = ((uint8_t*)key)[i];
  }
}

// update context
void b2b_update (b2b_ctx *ctx, 
  void *input, uint32_t len)
{
  uint32_t i;
  uint8_t *p=(uint8_t*)input;
  
  if (len==0) return;
  
  // update buffer and state
  for (i=0; i<len; i++) {
    if (ctx->index==128) {
      ctx->len.v64 += ctx->index;
      b2b_compress (ctx, 0);
      ctx->index = 0;
    }
    ctx->buffer.v8[ctx->index++] = p[i];
  }
}

// finalize
void b2b_final (void* out, b2b_ctx *ctx)
{
  uint32_t i;
  
  // update length with current index to buffer
  ctx->len.v64 += ctx->index;
  
  // zero remainder of buffer
  while (ctx->index < 128) {
    ctx->buffer.v8[ctx->index++] = 0;
  }

  // compress last block
  b2b_compress (ctx, 1);

  // copy state to output
  for (i=0; i<ctx->outlen; i++) {
    ((uint8_t*)out)[i] = ctx->state.v8[i];
  }
}
