
// test unit for tea.c
// odzhan
#include "tea.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

char *test_keys[] =
{ "00000000000000000000000000000000",
  "0000000000000000000000000a3aea41",
  "00000000000000000a3aea4129788e4e",
  "000000000a3aea4129788e4e5ea98bc8",
  "f2bf605c1cd07270ebc51345ab383a8f" };
     
char *test_plaintexts[] =
{ "0000000000000000",
  "40a9ba9400000000",
  "d836827d00000000",
  "02acede700000000",
  "adc4d980359689cf" };
    
char *test_ciphertexts[] =
{ "0a3aea4140a9ba94",
  "29788e4ed836827d",
  "5ea98bc802acede7",
  "af284eb88820b6b6",
  "b3f1b02b11ed23c0" };
   
typedef struct enc_t {
  char *pt;
  char *ct;
} enc;
   
enc enc_pt[] =
{ { "0000000000000000", "0a3aea4140a9ba94" },
  { "0a3aea4140a9ba94", "b9354a861ea75492" },
  { "b9354a861ea75492", "1dbae8aaae2bba9a" },
  { "1dbae8aaae2bba9a", "0eb60bc90296522d" }};

size_t hex2bin (void *bin, char hex[]) {
  size_t  len, i;
  int     x;
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

void dump_hex(void *b, int len)
{
  int i;
  for (i=0; i<len; i++) {
    printf ("%02x", ((uint8_t*)b)[i]);
  }
}

void enc_test (void)
{
  int i;
  tea_blk pt, ct, t;
  tea_key key;
  
  printf ("\nRunning Encrypt tests");
  memset (key.b, 0, TEA_KEY_LEN);

  for (i=0; i<sizeof(pt)/sizeof(enc); i++)
  {
    if (!hex2bin (pt.b, enc_pt[i].pt)) printf ("plaintext error");
    if (!hex2bin (ct.b, enc_pt[i].ct)) printf ("ciphertext error");
    
    TEA_Encrypt (&pt, &key, TEA_ENCRYPT);
    
    if (memcmp (pt.b, ct.b, TEA_BLK_LEN) != 0) {
      printf ("\nFailed for \"%s\"", test_plaintexts[i]);
      printf ("\nGot : ");
      dump_hex(pt.b, 8);
      printf (" instead of ");
      dump_hex(ct.b, 8);
    }
  }
}

int run_tests (void)
{
  int i, fails=0;
  tea_blk pt, ct;
  tea_key key;
  
  for (i=0; i<sizeof(test_plaintexts)/sizeof(char*); i++)
  {
    if (!hex2bin (key.b, test_keys[i])) printf  ("key error");
    if (!hex2bin (ct.b, test_ciphertexts[i])) printf ("ciphertext error");
    if (!hex2bin (pt.b, test_plaintexts[i])) printf ("plaintext error");
    
    TEA_Encrypt (&pt, &key, TEA_ENCRYPT);
    
    if (memcmp (pt.b, ct.b, TEA_BLK_LEN) != 0) {
      printf ("\nFailed for \"%s\"", test_plaintexts[i]);
      printf ("\nGot : ");
      dump_hex(pt.b, 8);
      printf (" instead of ");
      dump_hex(ct.b, 8);
      
      ++fails;
    }
  }
  return fails;
}

// do not use this for real application
// initialize with more appropriate source of information!
// time() is not random but you should know that already..
void fill_rand (uint8_t out[], int len) {
  int i;
  srand(time(0));
  for (i=0; i<len; i++) {
    out[i] = (uint8_t)rand();
  }
}

void dump (char txt[], uint8_t in[], int len) {
  int i;
  printf ("\n%s : ", txt);
  for (i=0; i<len; i++) {
    printf ("%02x", in[i]);
  }
}

int main (int argc, char *argv[])
{
  run_tests();
  enc_test();
  return 0;
}
