

// Cipher Block Chaining (CBC)

#include "aes.h"

void aes_cbc_enc (aes_ctx *ctx, void *input, 
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
    // xor with input
    r=memxor (out, in, &t, len);
    // encrypt
    aes_encrypt (ctx, out, AES_ENCRYPT);
    // copy result to iv
    blkcpy (iv, out);
    len -= r;
    in++;
    out++;
  }
}

void aes_cbc_dec (aes_ctx *ctx, void *input, 
  void *output, uint32_t len, void *ivec)
{
  aes_blk  t;
  aes_blk  *in  = (aes_blk*)input;
  aes_blk  *out = (aes_blk*)output;
  aes_blk  *iv  = (aes_blk*)ivec;
  uint32_t r;

  while (len > 0) {
    // copy input to temp buf
    blkcpy (&t, in);
    // decrypt input
    aes_encrypt (ctx, in, AES_DECRYPT);
    // xor with iv
    r=memxor (out, in, iv, len);
    // copy temp buf to iv
    blkcpy (iv, &t);
    len -= r;
    in++;
    out++;
  }
}

