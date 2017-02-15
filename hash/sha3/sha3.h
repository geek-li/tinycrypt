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

#ifndef SHA3_H
#define SHA3_H

#include <stdint.h>

#ifndef INTRINSICS

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)

#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTR8(v, n) ROTL8(v, 8 - (n))
#define ROTR16(v, n) ROTL16(v, 16 - (n))
#define ROTR32(v, n) ROTL32(v, 32 - (n))

#else
  
#define memset(x, v, n) __stosb (x, v, n)
#define memcpy(d, s, n) __movsb (d, s, n)

#define ROTL32(v, n) _rotl (v, n)
#define ROTR32(v, n) _rotr (v, n)

#endif

#define SWAP16(v) \
  ROTL16(v, 8)

#if defined (__INTEL_COMPILER)
#define SWAP32(x) _bswap(x)
#elif defined(_MSC_VER)
#define SWAP32(x) _byteswap_ulong(x)
#elif defined(__GNU_C)
#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))
#endif

#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))
  
#define ROTR64(v, n) ROTL64(v, 64 - (n))

#define SWAP64(v) \
  ((ROTL64(v,  8) & 0x000000FF000000FFULL) | \
   (ROTL64(v, 24) & 0x0000FF000000FF00ULL) | \
   (ROTL64(v, 40) & 0x00FF000000FF0000ULL) | \
   (ROTL64(v, 56) & 0xFF000000FF000000ULL))
   
#define SHA3_ROUNDS             24
#define SHA3_STATE_LEN          25

#define SHA3_224_DIGEST_LENGTH  28
#define SHA3_224                28
#define SHA3_224_CBLOCK        144

#define SHA3_256_DIGEST_LENGTH  32
#define SHA3_256                32
#define SHA3_256_CBLOCK        136

#define SHA3_384_DIGEST_LENGTH  48
#define SHA3_384                48
#define SHA3_384_CBLOCK        104

#define SHA3_512_DIGEST_LENGTH  64
#define SHA3_512                64
#define SHA3_512_CBLOCK         72

typedef union sha3_st_t {
  uint8_t  v8[SHA3_STATE_LEN*8];
  uint16_t v16[SHA3_STATE_LEN*4];
  uint32_t v32[SHA3_STATE_LEN*2];
  uint64_t v64[SHA3_STATE_LEN];
} sha3_st;

#pragma pack(push, 1)
typedef struct _SHA3_CTX {
  uint32_t olen;
  uint32_t blen;
  uint32_t idx;
  
  sha3_st s;
} SHA3_CTX;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

  void SHA3_Init (SHA3_CTX *, int);
  void SHA3_Update (SHA3_CTX*, void *, uint32_t);
  void SHA3_Final (void*, SHA3_CTX*);

#ifdef __cplusplus
}
#endif

#ifdef USE_ASM
  #define SHA3_Init(x, y) SHA3_Initx(x, y)
  #define SHA3_Update(x, y, z) SHA3_Updatex(x, y, z)
  #define SHA3_Final(x, y) SHA3_Finalx(x, y)
#endif

#endif