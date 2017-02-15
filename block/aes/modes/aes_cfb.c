

// Cipher Feedback (CFB)

#include "aes.h"

void aes_cfb_enc (aes_ctx *ctx, void *input, 
  void *output, uint32_t len, void *ivec)
{
  aes_blk t;
  aes_blk *in  = (aes_blk*)input;
  aes_blk *out = (aes_blk*)output;
  aes_blk *iv  = (aes_blk*)ivec;
  uint32_t r;
  
  while (len > 0) {
    // encrypt iv
    aes_encrypt (ctx, iv, AES_ENCRYPT);
    // xor ciphertext with input
    r=memxor (out, in, iv, len);
    // copy ciphertext to iv
    blkcpy (iv, out);
    len -= r;
    in++;
    out++;
  }
}

