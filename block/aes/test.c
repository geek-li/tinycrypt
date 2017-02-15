

// Test unit for AES-256 ECB mode
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

#include "aes.h"

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
  int i, j, plen, clen, klen, fails=0;

  aes_blk ct1, ct2, pt1, pt2;
  uint8_t key[32];           // 256-bit key
  aes_ctx ctx;
  
  for (i=0; i<sizeof(ecb_keys)/sizeof(char*); i++)
  { 
    klen=hex2bin (key, ecb_keys[i]);
    clen=hex2bin (ct1.v8, ecb_ct[i]);
    plen=hex2bin (pt1.v8, ecb_tv[i]);
    
    aes_setkey (&ctx, &key);
    aes_encrypt (&ctx, &pt1, AES_ENCRYPT);

    if (memcmp (ct1.v8, pt1.v8, clen)==0) {
      printf ("\nEncryption passed test #%i - %08X %08X", 
        (i+1), pt1.v32[0], ct1.v32[0]);
      
      plen=hex2bin (pt2.v8, ecb_tv[i]);
      
      aes_encrypt (&ctx, &pt1, AES_DECRYPT);
      
      if (memcmp (pt1.v8, pt2.v8, plen)==0) {
        printf ("\nDecryption OK!");
      } else {
        printf ("\nDecryption failed.. %08X %08X", pt1.v32[0], pt2.v32[0]);
      }
    } else {
      fails++;
      printf ("\nFailed test #%i : "
          "Got %08X %08X %08X %08X instead of %08X %08X %08X %08X", (i+1), 
          pt1.v32[0], pt1.v32[1], pt1.v32[2], pt1.v32[3],
          ct1.v32[0], ct1.v32[1], ct1.v32[2], ct1.v32[3]);
    }
  }
  return fails;
}

int main (int argc, char *argv[])
{
  ecb_test();
  return 0;
}
