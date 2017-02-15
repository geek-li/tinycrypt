

#include "aes.h"

// xor dst blk by src
uint32_t memxor (aes_blk *dst, aes_blk *s1, aes_blk *s2, uint32_t len)
{
  uint8_t i;
  
  len=(len>AES_BLK_LEN) ? AES_BLK_LEN:len;
  
  for (i=0; i<len; i++) {
    dst->v8[i] = s1->v8[i] ^ s2->v8[i];
  }
  return len;
}

// xor dst blk by src
void blkxor (aes_blk *dst, aes_blk *src)
{
  uint8_t i;

  for (i=0; i<AES_BLK_LEN; i++) {
    dst->v8[i] ^= src->v8[i];
  }
}

// copy src blk to dst
void blkcpy (aes_blk *dst, aes_blk *src)
{
  uint8_t i;

  for (i=0; i<AES_BLK_LEN; i++) {
    dst->v8[i] = src->v8[i];
  }
}

// clear block
void blkclr (aes_blk *blk)
{
  uint8_t i;

  for (i=0; i<AES_BLK_LEN; i++) {
    blk->v8[i] = 0;
  }
}
