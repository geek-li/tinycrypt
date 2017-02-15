

// AES test unit in C
// Odzhan

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

// ecb tests
char *ecb_keys[] = 
{ "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4" };

char *ecb_tv[] =
{ "6bc1bee22e409f96e93d7e117393172a",
  "ae2d8a571e03ac9c9eb76fac45af8e51",
  "30c81c46a35ce411e5fbc1191a0a52ef",
  "f69f2445df4f9b17ad2b417be66c3710" };
            
char *ecb_ct[] =
{ "f3eed1bdb5d2a03c064b5a7e3db181f8",
  "591ccb10d410ed26dc5ba74a31362870",
  "b6ed21b99ca6f4f9f153e7b1beafed1d",
  "23304b7a39f9f3ff067d8d8f9e24ecc7" };
  
// cbc tests
char *cbc_key[]=
{"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"};

char *cbc_iv[]=
{"000102030405060708090a0b0c0d0e0f",
 "f58c4c04d6e5f1ba779eabfb5f7bfbd6"};

char *cbc_tv[]=
{"6bc1bee22e409f96e93d7e117393172a",
 "ae2d8a571e03ac9c9eb76fac45af8e51"};

char *cbc_ct[]=
{"f58c4c04d6e5f1ba779eabfb5f7bfbd6",
 "9cfc4e967edb808d679f777bc6702c7d"};

// cfb tests
char *cfb_key[]=
{"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"};

char *cfb_iv[]=
{"000102030405060708090a0b0c0d0e0f",
 "dc7e84bfda79164b7ecd8486985d3860"};

char *cfb_tv[]=
{"6bc1bee22e409f96e93d7e117393172a",
 "ae2d8a571e03ac9c9eb76fac45af8e51"};

char *cfb_ct[]=
{"dc7e84bfda79164b7ecd8486985d3860",
 "39ffed143b28b1c832113c6331e5407b"};
 
// ctr tests
char *ctr_key[]=
{"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"};

char *ctr_iv[]=
{"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
 "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"};

char *ctr_tv[]=
{"6bc1bee22e409f96e93d7e117393172a",
 "ae2d8a571e03ac9c9eb76fac45af8e51"};

char *ctr_ct[]=
{"601ec313775789a5b7a7f504bbf3d228",
 "f443e3ca4d62b59aca84e990cacaf5c5"};
 
// ofb tests
char *ofb_key[]=
{"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"};

char *ofb_iv[]=
{"000102030405060708090A0B0C0D0E0F",
 "B7BF3A5DF43989DD97F0FA97EBCE2F4A"};

char *ofb_tv[]=
{"6bc1bee22e409f96e93d7e117393172a",
 "ae2d8a571e03ac9c9eb76fac45af8e51"};

char *ofb_ct[]=
{"dc7e84bfda79164b7ecd8486985d3860",
 "4febdc6740d20b3ac88f6ad82a4fb08d"};
  
#include "aes.h"
//#include "sha2.h"

int _isxdigit (int c)
{
  return (c >= '0' && c <= '9') || 
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

int _isprint (int c)
{
  return 1;
}
uint32_t hex2bin (void *bin, char hex[]) {
  uint32_t len, i;
  uint32_t x;
  uint8_t *p=(uint8_t*)bin;
  
  len = strlen (hex);
  
  if ((len & 1) != 0) {
    return 0; 
  }
  
  for (i=0; i<len; i++) {
    if (_isxdigit((int)hex[i]) == 0) {
      return 0; 
    }
  }
  
  for (i=0; i<len / 2; i++) {
    sscanf (&hex[i * 2], "%2x", &x);
    p[i] = (uint8_t)x;
  } 
  return len / 2;
} 

void bin2scr (void *bin, uint32_t len) 
{
  uint32_t i, ofs;
  uint8_t c;
  uint8_t *mem=(uint8_t*)bin;
  
  printf ("\n");
  
  for (ofs=0; ofs<len; ofs+=16) 
  {
    printf ("\n%08X", ofs);
    for (i=0; i<16 && ofs+i < len; i++) {
      printf (" %02x", mem[ofs + i]);
    }
    while (i++ < 16) {
      printf ("   ");
    }
    printf ("    ");

    for (i=0; i<16 && ofs+i < len; i++) {
      c=mem[ofs+i];
      printf ("%c", (c=='\t' || !_isprint (c)) ? '.' : c);
    }
  }
}

int ecb_test (void)
{
  int i, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, pt1, pt2;
  uint8_t key[32];
  aes_ctx ctx;
  
  for (i=0; i<sizeof (ecb_keys)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, ecb_keys[i]);
    clen=hex2bin (ct1.v8, ecb_ct[i]);
    plen=hex2bin (pt1.v8, ecb_tv[i]);
    
    aes_setkey (&ctx, key);
    aes_ecb_enc (&ctx, &pt1, &ct2, plen);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nECB Encryption mode #%i OK", (i+1));
      
      memcpy (pt2.v8, ct2.v8, AES_BLK_LEN);
      
      aes_ecb_dec (&ctx, &ct2, &pt2, clen);
      
      if (memcmp (pt1.v8, pt2.v8, plen)==0) {
        printf ("\nECB Decryption mode OK!");
      } else {
        printf ("\nDecryption failed.. %08X %08X", pt1.v32[0], pt2.v32[0]);
      }
    } else {
      fails++;
      printf ("\nFailed test #%i : "
          "Got %08X %08X instead of %08X %08X for %08X %08X", (i+1), 
          pt1.v32[0], pt1.v32[1], ct1.v32[0], ct1.v32[1],
          pt1.v32[0], pt1.v32[1]);
    }
  }
  return fails;
}

int cbc_test (void)
{
  int i, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, tv, iv;
  uint8_t key[32];
  aes_ctx ctx;
  
  for (i=0; i<sizeof (cbc_key)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, cbc_key[i]);
    clen=hex2bin (ct1.v8, cbc_ct[i] );
    plen=hex2bin (tv.v8,  cbc_tv[i] );
    
    hex2bin (iv.v8, cbc_iv[i]);
    
    aes_setkey (&ctx, key);  
    aes_cbc_enc (&ctx, &tv, &ct2, plen, &iv);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nPassed CBC test #%i", (i+1));
    } else {
      fails++;
      printf ("\nFailed CBC test #%i : %08X %08X != %08X %08X", 
        i+1, 
        ct2.v32[0], ct2.v32[1],
        ct1.v32[0], ct1.v32[1]);
    }
  }
  return fails;
}

int cfb_test (void)
{
  int i, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, tv, iv;
  uint8_t key[32];
  aes_ctx ctx;
  
  for (i=0; i<sizeof (cfb_key)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, cfb_key[i]);
    clen=hex2bin (ct1.v8, cfb_ct[i] );
    plen=hex2bin (tv.v8,  cfb_tv[i] );
    
    hex2bin (iv.v8, cfb_iv[i]);
    
    aes_setkey (&ctx, key);  
    aes_cfb_enc (&ctx, &tv, &ct2, plen, &iv);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nPassed CFB test #%i", (i+1));
    } else {
      fails++;
      printf ("\nFailed CFB test #%i : %08X %08X != %08X %08X", 
        i+1, 
        ct2.v32[0], ct2.v32[1],
        ct1.v32[0], ct1.v32[1]);
    }
  }
  return fails;
}

int ctr_test (void)
{
  int i, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, ct3, tv, iv, iv2, ctr;
  uint8_t key[32];
  aes_ctx ctx;
  
  memset (&ctr, 0, sizeof (ctr));
  hex2bin (iv.v8, ctr_iv[0]);

  for (i=0; i<sizeof (ctr_key)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, ctr_key[i]);
    clen=hex2bin (ct1.v8, ctr_ct[i] );
    plen=hex2bin (tv.v8,  ctr_tv[i] );
    
    aes_setkey (&ctx, key);  
    aes_ctr_enc (&ctx, &tv, &ct2, plen, &iv);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nCTR Encryption succeeded #%i", (i+1));
      
      hex2bin (iv2.v8, ctr_iv[0]);
      aes_ctr_enc (&ctx, &ct2, &ct3, plen, &iv2);

      if (memcmp (tv.v8, ct3.v8, clen)==0) {
        printf ("\nCTR Decryption succeeded");
      }
    } else {
      fails++;
      printf ("\nFailed CTR test #%i : %08X %08X != %08X %08X", 
        i+1, 
        ct2.v32[0], ct2.v32[1],
        ct1.v32[0], ct1.v32[1]);
    }
  }
  return fails;
}

int ofb_test (void)
{
  int i, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, tv, iv;
  uint8_t key[32];
  aes_ctx ctx;
  
  for (i=0; i<sizeof (ofb_key)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, ofb_key[i]);
    clen=hex2bin (ct1.v8, ofb_ct[i] );
    plen=hex2bin (tv.v8,  ofb_tv[i] );
    
    hex2bin (iv.v8, ofb_iv[i]);
    
    aes_setkey (&ctx, key);  
    aes_ofb_enc (&ctx, &tv, &ct2, plen, &iv);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nPassed OFB test #%i", (i+1));
    } else {
      fails++;
      printf ("\nFailed OFB test #%i : %08X %08X != %08X %08X", 
        i+1, 
        ct2.v32[0], ct2.v32[1],
        ct1.v32[0], ct1.v32[1]);
    }
  }
  return fails;
}

int main (int argc, char *argv[])
{
  if (!ecb_test()) {
    printf ("\n  [ ECB self-test OK!\n");
  }
  if (!cbc_test()) {
    printf ("\n  [ CBC self-test OK!\n");
  }    
  if (!cfb_test()) {
    printf ("\n  [ CFB self-test OK!\n");
  }    
  if (!ctr_test()) {
    printf ("\n  [ CTR self-test OK!\n");
  }    
  if (!ofb_test()) {
    printf ("\n  [ OFB self-test OK!\n");
  }
  return 0;
}
