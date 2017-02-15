

// RC5 test in C
// Odzhan

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "rc5.h"

typedef struct _rc5_tv {
  char *key;
  char *pt;
  char *ct;
} rc5_tv;

rc5_tv tv[2]=
{
  {"000102030405060708090A0B0C0D0E0F", "96950DDA654A3D62", "0011223344556677"},
  {"00000001000000000000000000000000", "0000000000000000", "AEDA56A5190042CE"}
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

int main (int argc, char *argv[])
{
  int i, e;
  RC5_CTX ctx;
  uint8_t pt1[8], pt2[8], ct1[8], ct2[8], key[16];
  
  for (i=0; i<2; i++)
  {
    hex2bin (key, tv[i].key);
    hex2bin (pt1, tv[i].pt);
    hex2bin (ct1, tv[i].ct);
    
    rc5_setkeyx (&ctx, key);
    rc5_cryptx (&ctx, pt1, ct2, RC5_ENCRYPT);
    
    e=memcmp (ct1, ct2, RC5_BLK_LEN);
    printf ("\n\nRC5 encryption test #%i %s", 
      (i+1), e==0?"passed":"failed");

    rc5_cryptx (&ctx, ct2, pt2, RC5_DECRYPT);
    
    e=memcmp (pt1, pt2, RC5_BLK_LEN);
    printf ("\nRC5 decryption test #%i %s", 
      (i+1), e==0?"passed":"failed");
  }
  return 0;
}
