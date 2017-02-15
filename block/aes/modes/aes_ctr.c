

// Counter (CTR)

#include "aes.h"

void inc_ctr (aes_blk *ctr)
{
  int i;
  
  for (i=AES_BLK_LEN-1; i>=0; i--) {
    ctr->v8[i]++;
    if (ctr->v8[i]) {
      break;
    }
  }
}

void aes_ctr_enc (aes_ctx *ctx, void *input, 
  void *output, uint32_t len, void *ivec)
{
  aes_blk t;
  aes_blk *in  = (aes_blk*)input;
  aes_blk *out = (aes_blk*)output;
  aes_blk *iv  = (aes_blk*)ivec;
  uint32_t r;

  while (len > 0) {
    // copy iv to temporary buffer
    blkcpy (&t, iv);
    // encrypt t
    aes_encrypt (ctx, &t);
    // xor with plaintext 
    r=memxor (out, in, &t, len);
    // increase iv
    inc_ctr (iv);
    len -= r;
    in++;
    out++;
  }
}
