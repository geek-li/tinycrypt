


void SHA1_HMAC (void *text, size_t text_len, 
  void *key, size_t key_len, void* dgst)
{
    SHA_CTX  ctx;
    uint8_t  k_ipad[64];
    uint8_t  k_opad[64];
    uint8_t  tk[SHA_DIGEST_LENGTH];
    uint32_t i;
    uint8_t  *k=(uint8_t*)key;

    if (key_len > 64) {
      SHA1_Init (&ctx);
      SHA1_Update (&ctx, key, key_len);
      SHA1_Final (tk, &ctx);

      key = tk;
      key_len = SHA_DIGEST_LENGTH;
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
    SHA1_Init (&ctx);                             // init context for 1st pass
    SHA1_Update (&ctx, k_ipad, 64);               // start with inner pad
    SHA1_Update (&ctx, text, text_len);           // then text of datagram
    SHA1_Final (dgst, &ctx);                      // finish up 1st pass
    /**
     * perform outer
     */
    SHA1_Init (&ctx);                             // init context for 2nd pass
    SHA1_Update (&ctx, k_opad, 64);               // start with outer pad
    SHA1_Update (&ctx, dgst, SHA_DIGEST_LENGTH);  // then results of 1st hash
    SHA1_Final (dgst, &ctx);                      // finish up 2nd pass
}
