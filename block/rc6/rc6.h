

// RC6 in C
// Odzhan

#ifndef RC6_H
#define RC6_H

#include <stdint.h>

#define RC6_ROUNDS 20
#define RC6_KR     (2*(RC6_ROUNDS+2))
#define RC6_P      0xB7E15163
#define RC6_Q      0x9E3779B9

#define RC6_ENCRYPT 1
#define RC6_DECRYPT 0

typedef struct _RC6_KEY {
  uint32_t x[RC6_KR];
} RC6_KEY;

typedef union _rc6_blk_t {
  uint8_t v8[16];
  uint16_t v16[8];
  uint32_t v32[4];
  uint64_t v64[2];
} rc6_blk;

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

#ifdef __cplusplus
extern "C" {
#endif

  // asm prototype
  void rc6_setkeyx (RC6_KEY*, void*, uint32_t);
  void rc6_cryptx (RC6_KEY*, void*, void*, int);
  
  // C prototype
  void rc6_setkey (RC6_KEY*, void*, uint32_t);
  void rc6_crypt (RC6_KEY*, void*, void*, int);

#ifdef __cplusplus
}
#endif

#ifdef USE_ASM
#define rc6_setkey(x, y, z) rc6_setkeyx (x, y, z)
#define rc6_crypt(w, x, y, z) rc6_cryptx (w, x, y, z)
#endif

#endif