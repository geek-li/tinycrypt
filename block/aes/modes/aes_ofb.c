

// Output Feedback (OFB)

#include "aes.h"

void aes_ofb_enc (aes_ctx *ctx, void *input, 
  void *output, uint32_t len, void *ivec)
{
  aes_blk t;
  aes_blk *in  = (aes_blk*)input;
  aes_blk *out = (aes_blk*)output;
  aes_blk *iv  = (aes_blk*)ivec;
  uint32_t r;
  
  while (len > 0) {
    // copy iv
    blkcpy (&t, iv);
    // encrypt iv
    aes_encrypt (ctx, &t, AES_ENCRYPT);
    // copy result to iv
    blkcpy (iv, &t);
    // xor with input
    r=memxor (out, in, &t, len);
    len -= r;
    in++;
    out++;
  }
}

