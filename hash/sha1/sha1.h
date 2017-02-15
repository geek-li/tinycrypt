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
  
#ifndef SHA1_H
#define SHA1_H

#if defined (INTRINSICS) && !defined(_MSC_VER)
#error Sorry, but INTRINSICS are currently only supported by Microsoft and INTEL compilers.
#endif

#include <stdint.h>
#include <string.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#define SWAP32(v) _byteswap_ulong(v)
#define SWAP64(v) _byteswap_uint64(v)

#define memcpy(d, s, n) __movsb (d, s, n)
#define memset(x, v, n) __stosb (x, v, n)

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

/*
#define SWAP16(v) \
  ROTL16(v, 8)

#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))

#define SWAP64(v) \
  ((ROTL64(v,  8) & 0x000000FF000000FFULL) | \
   (ROTL64(v, 24) & 0x0000FF000000FF00ULL) | \
   (ROTL64(v, 40) & 0x00FF000000FF0000ULL) | \
   (ROTL64(v, 56) & 0xFF000000FF000000ULL))
*/

#define SHA1_CBLOCK        64
#define SHA1_DIGEST_LENGTH 20
#define SHA1_STATE_LEN     32
#define SHA1_LBLOCK        SHA1_DIGEST_LENGTH/4

#pragma pack(push, 1)
typedef struct _SHA1_CTX {
  union {
    uint8_t  b[SHA1_DIGEST_LENGTH];
    uint32_t w[SHA1_LBLOCK];
  }s;
  union {
    uint8_t  b[SHA1_CBLOCK];
    uint32_t w[SHA1_CBLOCK/4];
    uint64_t q[SHA1_CBLOCK/8];
  } buf;
  uint64_t len;
} SHA1_CTX;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

  void SHA1_Init (SHA1_CTX*);
  void SHA1_Update (SHA1_CTX*, void*, uint32_t);
  void SHA1_Final (void*, SHA1_CTX*);

#ifdef __cplusplus
}
#endif

#endif
