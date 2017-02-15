

// MD2 in C
// Odzhan

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef OSSL
#include <openssl/md2.h>
#else
#include "md2.h"
#endif

void dump (char *s, uint8_t h[])
{
  int i;
  printf ("\n%-10s ", s);
  for (i=0; i<MD2_DIGEST_LENGTH; i++) {
    printf ("%02x", h[i]);
  }
}

char *text[] =
{ "The quick brown fox jumps over the lazy dog",
  "The quick brown fox jumps over the lazy cog",
  "",
  "a",
  "abc",
  "message digest",
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
  "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

char *md2_dgst[] =
{ "03d85a0d629d2c442e987525319fc471",
  "6b890c9292668cdbbfda00a4ebf31f05",
  "8350e5a3e24c153df2275c9f80692773",
  "32ec01ec4a6dac72c0ab96fb34c0b5d1",
  "da853b0d3f88d99b30283a69e6ded6bb",
  "ab4f496bfb2a530b219ff33031fe06b0",
  "4e8ddff3650292ab5a4108c3aa47940b",
  "da33def2a42df13975352846c30338cd",
  "d5976f79d83d3a0dc9806c3c66f3efd8"
};

size_t hex2bin (void *bin, char hex[]) {
  size_t len, i;
  int x;
  uint8_t *p=(uint8_t*)bin;
  
  len = strlen (hex);
  
  if ((len & 1) != 0) {
    return 0; 
  }
  
  for (i=0; i<len; i++) {
    if (isxdigit((int)hex[i]) == 0) {
      return 0; 
    }
  }
  
  for (i=0; i<len / 2; i++) {
    sscanf (&hex[i * 2], "%2x", &x);
    p[i] = (uint8_t)x;
  } 
  return len / 2;
} 

/************************************************
 *
 * finalize.
 *
 ************************************************/
void MD2_HMAC (void *text, size_t text_len, 
  void *key, size_t key_len, void *dgst)
{
  MD2_CTX ctx;
  uint8_t k_ipad[65], k_opad[65], tk[MD2_DIGEST_LENGTH];
  size_t  i;
  uint8_t *k=(uint8_t*)key;

  if (key_len > 64) {
    MD2_Init (&ctx);
    MD2_Update (&ctx, key, key_len);
    MD2_Final (tk, &ctx);

    k = tk;
    key_len = MD2_DIGEST_LENGTH;
  }

  memset (k_ipad, 0x36, sizeof (k_ipad));
  memset (k_opad, 0x5c, sizeof (k_opad));

  /** XOR key with ipad and opad values */
  for (i=0; i<key_len; i++) {
    k_ipad[i] ^= k[i];
    k_opad[i] ^= k[i];
  }
  /**
   * perform inner 
   */
  MD2_Init (&ctx);                       /* init context for 1st pass */
  MD2_Update (&ctx, k_ipad, 64);         /* start with inner pad */
  MD2_Update (&ctx, text, text_len);     /* then text of datagram */
  MD2_Final (dgst, &ctx);                /* finish up 1st pass */
  /**
   * perform outer
   */
  MD2_Init (&ctx);                       /* init context for 2nd pass */
  MD2_Update (&ctx, k_opad, 64);         /* start with outer pad */
  MD2_Update (&ctx, dgst, MD2_DIGEST_LENGTH); /* then results of 1st hash */
  MD2_Final (dgst, &ctx);                /* finish up 2nd pass */
}

void MD2 (void *in, size_t len, void *out)
{
  MD2_CTX ctx;
  
  MD2_Init (&ctx);
  MD2_Update (&ctx, in, len);
  MD2_Final (out, &ctx);
}

int run_tests (void)
{
  uint8_t dgst[MD2_DIGEST_LENGTH], tv[MD2_DIGEST_LENGTH];
  int i, fails=0;
  MD2_CTX ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    MD2_Init (&ctx);
    MD2_Update (&ctx, text[i], strlen(text[i]));
    MD2_Final (dgst, &ctx);
    
    hex2bin (tv, md2_dgst[i]);
    
    if (memcmp (dgst, tv, MD2_DIGEST_LENGTH) != 0) {
      ++fails;
    }
  }
  return fails;
}

int main (int argc, char *argv[])
{
  char    *t, *k;
  uint8_t dgst[MD2_DIGEST_LENGTH];
  size_t  tlen, klen;
  int     fails;
  MD2_CTX ctx;
  
  if (argc < 2) {
    if (!(fails=run_tests())) {
      printf ("\nSelf-test OK!");
    } else {
      printf ("\nSelf-test failed with %i errors", fails);
    }
    return 0;
  }
  
  t=argv[1];
  tlen=strlen(t);
  k=argc==3 ? argv[2] : "";
  klen=strlen(k);
  
  MD2_Init (&ctx);
  MD2_Update (&ctx, t, tlen);
  MD2_Final (dgst, &ctx);
  dump ("MD2", dgst);
  
  MD2_HMAC (t, tlen, k, klen, dgst);
  dump ("HMAC-MD2", dgst);
  
  return 0;
}
