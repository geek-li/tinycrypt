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

#ifndef SHA2_H
#define SHA2_H

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
   
#define SHA256_CBLOCK        64
#define SHA256_DIGEST_LENGTH 32
#define SHA256_LBLOCK        SHA256_DIGEST_LENGTH/4

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#pragma pack(push, 1)
typedef struct _SHA256_CTX {
  uint64_t len;
  union {
    uint8_t  v8[SHA256_DIGEST_LENGTH];
    uint32_t v32[SHA256_DIGEST_LENGTH/4];
    uint64_t v64[SHA256_DIGEST_LENGTH/8];
  } s;
  union {
    uint8_t  v8[SHA256_CBLOCK];
    uint32_t v32[SHA256_CBLOCK/4];
    uint64_t v64[SHA256_CBLOCK/8];
  } buf;
} SHA256_CTX;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _SHA256X_CTX {
  uint32_t len;
  union {
    uint8_t  v8[SHA256_DIGEST_LENGTH];
    uint32_t v32[SHA256_DIGEST_LENGTH/4];
    uint64_t v64[SHA256_DIGEST_LENGTH/8];
  } s;
  union {
    uint8_t  v8[SHA256_CBLOCK];
    uint32_t v32[SHA256_CBLOCK/4];
    uint64_t v64[SHA256_CBLOCK/8];
  } buf;
} SHA256X_CTX;
#pragma pack(pop)

#define SHA512_CBLOCK        128
#define SHA512_DIGEST_LENGTH 64
#define SHA512_LBLOCK        SHA512_DIGEST_LENGTH/4

#pragma pack(push, 1)
typedef struct _SHA512_CTX {
  union {
    uint8_t  v8[SHA512_DIGEST_LENGTH];
    uint32_t v32[SHA512_DIGEST_LENGTH/4];
    uint64_t v64[SHA512_DIGEST_LENGTH/8];
  } state;
  union {
    uint8_t  v8[SHA512_CBLOCK];
    uint32_t v32[SHA512_CBLOCK/4];
    uint64_t v64[SHA512_CBLOCK/8];
  } buffer;
  uint64_t len[2];
} SHA512_CTX;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

  void SHA256_Init (SHA256_CTX*);
  void SHA256_Update (SHA256_CTX*, void*, uint32_t);
  void SHA256_Final (void*, SHA256_CTX*);

  void SHA512_Init (SHA512_CTX*);
  void SHA512_Update (SHA512_CTX*, void *, uint32_t);
  void SHA512_Final (void*, SHA512_CTX*);
  
  void SHA512_Transform (SHA512_CTX *ctx);
#ifdef __cplusplus
}
#endif

#ifdef USE_ASM
#define SHA256_Init(x) SHA256_Initx(x)
#define SHA256_Update(x, y, z) SHA256_Updatex(x,y,z)
#define SHA256_Final(x, y) SHA256_Finalx(x,y)

#define SHA512_Transform(x) SHA512_Transformx(x)
#endif

#endif