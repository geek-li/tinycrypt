
#include "des.h"

// xor dst blk by src
void memxor (des_blk *dst, des_blk *src, uint32_t len)
{
  uint8_t i;

  for (i=0; i<len; i++) {
    dst->v8[i] ^= src->v8[i];
  }
}

// xor dst blk by src
void blkxor (des_blk *dst, des_blk *src)
{
  uint8_t i;

  for (i=0; i<DES_BLK_LEN; i++) {
    dst->v8[i] ^= src->v8[i];
  }
}

// copy src blk to dst
void blkcpy (des_blk *dst, des_blk *src)
{
  uint8_t i;

  for (i=0; i<DES_BLK_LEN; i++) {
    dst->v8[i] = src->v8[i];
  }
}

// clear block
void blkclr (des_blk *blk)
{
  uint8_t i;

  for (i=0; i<DES_BLK_LEN; i++) {
    blk->v8[i] = 0;
  }
}

// perform encryption in CBC mode
void des_cbc_enc (void *key, void *data_in, 
  void *data_out, uint32_t len, void *v)
{
  des_blk t;
  des_blk *in=(des_blk*)data_in;
  des_blk *out=(des_blk*)data_out;
  des_blk *iv=(des_blk*)v;
  uint32_t r;
  
  // encrypt 64-bit blocks
  do {
    // clear t
    blkclr (&t);
    // copy 1 block or whatever is remaining to t
    r=(len > DES_BLK_LEN) ? DES_BLK_LEN : len;
    memcpy (t.v8, in->v8, r);
    // xor iv with t
    memxor (&t, iv, r);
    //des_enc (key, &t, out);
    blkcpy (iv, out);
    len -= r;
    in++;
    out++;
  } while (len != 0);
}

// perform decryption in CBC mode
uint32_t des_cbc_dec (void *key, void *data_in, 
  void *data_out, uint32_t len, void *v)
{
  des_blk  *in=(des_blk*)data_in;
  des_blk  *out=(des_blk*)data_out;
  des_blk  *iv=(des_blk*)v;
  uint32_t r;
  des_ctx ctx;
	
  // decrypt 64-bit blocks
  do {
    r=(len>DES_BLK_LEN) ? DES_BLK_LEN : len;
    // decrypt block
    des_enc (&ctx, in, out, DES_DECRYPT);
    // xor with iv
    memxor (out, iv, r);
    // copy cipher text into iv
    blkcpy (iv, in);
    len -= r;
    in++;
    out++;
  } while (len != 0);
  return r;
}

