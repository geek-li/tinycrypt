

#ifndef PRNG_H
#define PRNG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <intrin.h>

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
   
   
#pragma intrinsic (fabs, pow, sin, cos, pow, tan, sqrt, atan, log)

typedef union _vector_t { 
  uint8_t   v8[4];
  uint16_t v16[2];
  uint32_t v32;
  uint64_t v64;
} vector;

uint32_t sponge[128+1];

#define R(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

#ifdef __cplusplus
extern "C" {
#endif

  void prng_init (void);
  void prng_squeeze (void* out, size_t outlen);
  void prng_absorb (void* in, size_t inlen);

#ifdef __cplusplus
}
#endif
#endif