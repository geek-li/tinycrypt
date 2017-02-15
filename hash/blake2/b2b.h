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
  
#ifndef B2b_H
#define B2b_H

#include <stdint.h>

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))

#define ROTR8(v, n) ROTL8(v, 8 - (n))
#define ROTR16(v, n) ROTL16(v, 16 - (n))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define ROTR64(v, n) ROTL64(v, 64 - (n))

#define BLAKE2b_CBLOCK        128
#define BLAKE2b_DIGEST_LENGTH 64
#define BLAKE2b_LBLOCK        BLAKE2b_DIGEST_LENGTH/8
#define BLAKE2b_MAXKEY        64

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef union blake_st_t {
  uint8_t v8[BLAKE2b_DIGEST_LENGTH];
  uint16_t v16[BLAKE2b_DIGEST_LENGTH/2];
  uint32_t v32[BLAKE2b_DIGEST_LENGTH/4];
  uint64_t v64[BLAKE2b_DIGEST_LENGTH/8];
} blake_st;

typedef union blake_blk_t {
  uint8_t v8[BLAKE2b_CBLOCK];
  uint16_t v16[BLAKE2b_CBLOCK/2];
  uint32_t v32[BLAKE2b_CBLOCK/4];
  uint64_t v64[BLAKE2b_CBLOCK/8];
} blake_blk;

typedef union blake_len_t {
  uint8_t   v8[8];
  uint16_t v16[4];
  uint32_t v32[2];
  uint64_t v64;
} blake_len;

#pragma pack(push, 1)
typedef struct _b2b_ctx {
  blake_st  state;
  blake_st  b2s_iv;
  blake_blk buffer;
  blake_len len;
  uint32_t  index;
  uint32_t  outlen;
  uint32_t  rounds;
  uint16_t  v_idx[8];
  uint64_t  sigma[10];
} b2b_ctx;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

  void b2b_init (b2b_ctx*, uint32_t, void*, uint32_t, uint32_t);
  void b2b_update (b2b_ctx*, void*, uint32_t);
  void b2b_final (void*, b2b_ctx*);
#ifdef __cplusplus
}
#endif

#endif