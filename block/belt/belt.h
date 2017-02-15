/**
  Copyright © 2016 Odzhan. All Rights Reserved.

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
  
#ifndef BELT_H
#define BELT_H

#include <stddef.h>
#include <stdint.h>

#define BELT_ENCRYPT 0
#define BELT_DECRYPT 1

// 32-bit type
typedef union {
    uint8_t  b[4];
    uint32_t w;
} w32_t;

// 128-bit type
typedef struct {
    union {
      uint8_t  v8[16];
      uint32_t w[4];
      struct {
        uint32_t a, b, c, d;
      };
    };
} w128_t;

// 256-bit type
typedef struct {
    union {
      uint8_t  v8[32];
      uint32_t w[8];
      uint64_t q[4];
      struct {
        uint32_t a, b, c, d;
        uint32_t e, f, g, h;
      };
    };
} w256_t;

#ifdef INTRINSICS

#define ROTL32(v, n) _rotl(v, n)
#define ROTR32(v, n) _rotr(v, n)

#define memcpy(d, s, n) __movsb(d, s, n)
#define memset __stosb (d, v, n)

#else

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
#endif

#ifdef USE_ASM
#define belt_encrypt(x,y,z) belt_encryptx(x,y,z)
#endif

#define XCHG(x, y, t) (t) = (x); (x) = (y); (y) = (t);

#ifdef __cplusplus
extern "C" {
#endif

void belt_encrypt(void *blk, const void *ks, int enc);

#ifdef __cplusplus
}
#endif

#endif
