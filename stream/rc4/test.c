

// RC4 in C
// Odzhan

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "rc4.h"

//#include <openssl/rc4.h>

char *test_keys[] = 
{ "00000000000000000000000000000000",
  "0123456789abcdef0112233445566778",
  "000000000000000000000000000000000000000000000000",
  "0123456789abcdef0112233445566778899aabbccddeeff0",
  "0000000000000000000000000000000000000000000000000000000000000000",
  "0123456789abcdef0112233445566778899aabbccddeeff01032547698badcfe" };

char *test_plaintexts[] =
{ "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1",
  "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1",
  "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1" };

char *test_ciphertexts[] =
{ "de188941a3375d3a8a061e67576e926d",
  "797f2cb11a3f36e7c4b4eb9e2d3f3e00",
  "de188941a3375d3a8a061e67576e926d",
  "204c9de7b5e17e2b9a6e83502591b06c",
  "de188941a3375d3a8a061e67576e926d",
  "29799da497a400ba7939007ed3de39d2"};

void dump (char hdr[], void* buf, int len)
{
  size_t i;
  uint8_t *p=(uint8_t*)buf;
  
  printf ("\n%s : ", hdr);
  
  for (i=0; i<len; i++) {
    if ((i & 7)==0) putchar('\n');
    printf ("0x%02x, ", p[i]);
  }
}

void hexout (char hdr[], void* buf, int len)
{
  size_t i;
  uint8_t *p=(uint8_t*)buf;
  
  printf ("\n%s : ", hdr);
  
  for (i=0; i<len; i++) {
    printf ("%02x", p[i]);
  }
}

int hex2bin (void *bin, char hex[]) {
  int len, i, x;
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

void run_tests (void)
{
  int i, plen, clen, klen;
  uint8_t p1[128], c1[128], k[128];
  RC4_KEY ctx;
  
  for (i=0; i<sizeof (test_keys)/sizeof(char*); i++)
  {
    memset (p1, 0, sizeof (p1));
    memset (c1, 0, sizeof (c1));
    memset (k, 0, sizeof (k));
    
    klen=hex2bin (k,  test_keys[i]);
    clen=hex2bin (c1, test_ciphertexts[i]);
    plen=hex2bin (p1, test_plaintexts[i]);
    
    RC4_set_key (&ctx, klen, k);
    RC4 (&ctx, plen, p1, p1);
    
    if (memcmp (c1, p1, clen)==0) {
      printf ("\nPassed test # %i : plen=%i, keylen=%i-bit", (i+1), plen, klen*8);
    } else {
      printf ("\nFailed test # %i : plen=%i, keylen=%i-bit", (i+1), plen, klen*8);
      hexout ("required", c1, clen);
      hexout ("result  ",   p1, clen);
    }
  }
}

int main (void)
{
  run_tests();
  return 0;
}
