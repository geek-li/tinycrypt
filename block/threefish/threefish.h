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

#ifndef THREEFISH_H
#define THREEFISH_H

#define THREEFISH_ENCRYPT 0
#define THREEFISH_DECRYPT 1

#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>  
#endif  

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#ifndef INTRINSICS

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

#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))
   
#else

#if defined (__INTEL_COMPILER)
#define SWAP32(x) _bswap(x)
#elif defined(_MSC_VER)
#define SWAP32(x) _byteswap_ulong(x)
#elif defined(__GNU_C)
#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))
#endif

#define SWAP64(v) _byteswap_uint64(v)

#define memcpy(d, s, n) __movsb (d, s, n)
#define memset(x, v, n) __stosb (x, v, n)

#define ROTL8(v,n) _rotl8(v,n)
#define ROTL16(v,n) _rotl16(v,n)
#define ROTL32(v,n) _rotl(v,n)
#define ROTL64(v,n) _rotl64(v,n)

#define ROTR8(v,n) _rotr8(v,n)
#define ROTR16(v,n) _rotr16(v,n)
#define ROTR32(v,n) _rotr(v,n)
#define ROTR64(v,n) _rotr64(v,n)

#endif  
   
   
// for 256-bit keys
typedef struct{
  uint64_t k[5];
  uint64_t t[3];
} threefish_ctx_t;

#ifdef __cplusplus
extern "C" {
#endif

  void threefish_setkey (threefish_ctx_t*, const void*, const void*);
  void threefish_setkeyx (threefish_ctx_t*, const void*, const void*);
  void threefish_encrypt (const threefish_ctx_t*, void*, uint32_t);

#ifdef __cplusplus
}
#endif

#endif