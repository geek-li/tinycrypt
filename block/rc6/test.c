
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "rc6.h"

char *test_keys[] = 
{ "00000000000000000000000000000000",
  "0123456789abcdef0112233445566778",
  "00000000000000000000000000000000"
  "0000000000000000",
  "0123456789abcdef0112233445566778"
  "899aabbccddeeff0",
  "00000000000000000000000000000000"
  "00000000000000000000000000000000",
  "0123456789abcdef0112233445566778"
  "899aabbccddeeff01032547698badcfe" };

char *test_plaintexts[] =
{ "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1",
  "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1",
  "00000000000000000000000000000000",
  "02132435465768798a9bacbdcedfe0f1" };

char *test_ciphertexts[] =
{ "8fc3a53656b1f778c129df4e9848a41e",
  "524e192f4715c6231f51f6367ea43f18",
  "6cd61bcb190b30384e8a3f168690ae82",
  "688329d019e505041e52e92af95291d4",
  "8f5fbd0510d15fa893fa3fda6e857ec2",
  "c8241816f0d7e48920ad16a1674e5d48"};

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

void run_tests (void)
{
  size_t i, plen, clen, klen;
  uint8_t p1[32], p2[32], c1[32], c2[32], k[32];
  RC6_KEY rc6_key;
  
  for (i=0; i<sizeof (test_keys)/sizeof(char*); i++)
  {
    memset (p1, 0, sizeof (p1));
    memset (p2, 0, sizeof (p2));
    memset (c1, 0, sizeof (c1));
    memset (c2, 0, sizeof (c2));
    memset (k, 0, sizeof (k));
    
    klen=hex2bin (k, test_keys[i]);
    clen=hex2bin (c1, test_ciphertexts[i]);
    plen=hex2bin (p1, test_plaintexts[i]);
    
    rc6_setkey (&rc6_key, k, klen);
    rc6_crypt (&rc6_key, p1, c2, RC6_ENCRYPT);

    if (memcmp (c1, c2, plen)==0) {
      printf ("\nEncryption Passed test #%i", (i+1));
      rc6_crypt (&rc6_key, c2, p2, RC6_DECRYPT);
      if (memcmp (p1, p2, plen)==0) {
        printf ("\nDecryption passed test #%i", (i+1));
      } else {
        printf ("\nDecryption failed");
      }
    } else {
      printf ("\nFailed test #%i", (i+1));
    }
  }
}

int main (void)
{
  run_tests();
  return 0;
}
