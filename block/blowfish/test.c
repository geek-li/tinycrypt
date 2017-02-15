

// Blowfish in C
// Odzhan

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "blowfish.h"

char *test_keys[]={
  "0000000000000000",  
  "0113B970FD34F2CE",  
  "0170F175468FB5E6",                
  "43297FAD38E373FE",                
  "07A7137045DA2A16",                
  "04689104C2FD3B2F"                
};

char *test_pt[]={
  "0000000000000000",
  "059B5E0851CF143A",
  "0756D8E0774761D2",
  "762514B829BF486A",
  "3BDD119049372802",
  "26955F6835AF609A"
};

char *test_ct[]={
  "4EF997456198DD78",
  "48F4D0884C379918",
  "432193B78951FC98",
  "13F04154D69D1AE5",
  "2EEDDA93FFD39C79",
  "D887E0393C2DA6E3"
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

void run_tests(void)
{
  size_t i, plen, clen, klen;
  BF_LONG p1[2], c1[2], c2[2], k[32];
  BF_KEY bf_key;
  
  for (i=0; i<sizeof (test_keys)/sizeof(char*); i++)
  { 
    klen=hex2bin (k, test_keys[i]);
    clen=hex2bin (c1, test_ct[i]);
    plen=hex2bin (p1, test_pt[i]);
    
    bf_setkey (&bf_key, k, klen);
    bf_encrypt (&bf_key, p1, c2);

    if (memcmp (c1, c2, clen)==0) {
      printf ("\nPassed test #%i", (i+1));
    } else {
      printf ("\nFailed test #%i | Got %08X %08X instead of %08X %08X", 
      (i+1), c2[0], c2[1], c1[0], c1[1]);
    }
  }
}

int main (void)
{
  run_tests(); 
  return 0;
}


