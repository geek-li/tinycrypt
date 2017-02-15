/**
  Copyright © 2015 Odzhan. 
  Copyright © 2008 Daniel Otte
  
  All Rights Reserved.
  
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

#ifndef DES_H
#define DES_H

#include <stdint.h>

#define DES_BLK_LEN 8
#define DES_ROUNDS 16

#define ROTTABLE      0x7EFC 
#define ROTTABLE_INV  0x3F7E

#define DES_ENCRYPT 0
#define DES_DECRYPT 1

typedef union _des_t {
  uint8_t v8[DES_BLK_LEN];
  uint32_t v32[DES_BLK_LEN/4];
  uint64_t v64;
} des_blk;

typedef union _des_ctx_t {
	des_blk keys[16+1];
} des_ctx;

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#ifdef INTRINSICS
#define ROTL8(v, n) _rotl(v, n)
#define ROTL16(v, n) _rotl(v, n)
#define ROTL32(v, n) _rotl(v, n)
#define ROTL64(v, n) _rotl64(v, n)
#else
#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))
#endif

#define ROTR8(v, n) ROTL8(v, 8 - (n))
#define ROTR16(v, n) ROTL16(v, 16 - (n))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define ROTR64(v, n) ROTL64(v, 64 - (n))

#define SWAP16(v) \
  ROTL16(v, 8)

#ifdef INTRINSICS
#define SWAP32(v) _byteswap_ulong (v)
#else
#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))
#endif

#define SWAP64(v) \
  ((ROTL64(v,  8) & 0x000000FF000000FFULL) | \
   (ROTL64(v, 24) & 0x0000FF000000FF00ULL) | \
   (ROTL64(v, 40) & 0x00FF000000FF0000ULL) | \
   (ROTL64(v, 56) & 0xFF000000FF000000ULL))
   
/* the FIPS 46-3 (1999-10-25) name for triple DES is triple data encryption algorithm so TDEA.
* Also we only implement the three key mode  */

#ifdef __cplusplus
extern "C" {
#endif

  void des_str2key (void*, des_blk*);
	void des_setkey (des_ctx*, void *);
  void des_enc (des_ctx*, void*, void*, int);

  void des_str2keyx (void*, des_blk*);
	void des_setkeyx (des_ctx*, void *);
  void des_encx (des_ctx*, void*, void*, int);
	
  void des3_enc (void*, void*, void*, void*, void*);
  void des3_dec (void*, void*, void*, void*, void*);
  
void des_cbc_enc (void*, void*, void*, uint32_t, void*);
uint32_t des_cbc_dec (void*, void*, void*, uint32_t, void*);
  
#ifdef __cplusplus
}
#endif

#ifdef USE_ASM
#define des_str2key(x,y) des_str2keyx(x,y)
#define des_setkey(x,y) des_setkeyx(x,y)
#define des_enc(w,x,y,z) des_encx(w,x,y,z)
#endif

#endif
