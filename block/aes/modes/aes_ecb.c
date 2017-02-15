

// Electronic codebook (ECB)

#include "aes.h"

void aes_ecb_enc (aes_ctx *ctx, void *input, 
  void *output, uint32_t len)
{
  aes_blk t;
  aes_blk *in  = (aes_blk*)input;
  aes_blk *out = (aes_blk*)output;
  uint32_t r;
  
  while (len > 0) {
    // clear t
    blkclr (&t);
    // copy input to t
    r=memxor (&t, &t, in, len);
    // encrypt
    aes_encrypt (ctx, &t, AES_ENCRYPT);
    // copy to output
    blkcpy (out, &t);
    len -= r;
    in++;
    out++;
  }
}

void aes_ecb_dec (aes_ctx *ctx, void *input, 
  void *output, uint32_t len)
{
  aes_blk t;
  aes_blk *in  = (aes_blk*)input;
  aes_blk *out = (aes_blk*)output;
  uint32_t r;
  
  while (len > 0) {
    // clear t
    blkclr (&t);
    // copy input to t
    r=memxor (&t, &t, in, len);
    // encrypt
    aes_encrypt (ctx, &t, AES_DECRYPT);
    // copy to output
    blkcpy (out, &t);
    len -= r;
    in++;
    out++;
  }
}
