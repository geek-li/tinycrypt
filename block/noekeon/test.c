

#include "noekeon.h"

// direct mode test vectors
char tv_key[] = "000102030405060708090A0B0C0D0E0F";
char tv_pt[]  = "ED1F7C59EC86A49E2C6C22AE20B4AEDE";
char tv_ct[]  = "00112233445566778899AABBCCDDEEFF";

int _isxdigit (int c)
{
  return (c >= '0' && c <= '9') || 
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
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

void print_bytes(char *s, void *p, int len) {
  int i;
  printf("%s : ", s);
  for (i=0; i<len; i++) {
    printf ("%02x ", ((uint8_t*)p)[i]);
  }
  putchar('\n');
}

int main(void)
{
  uint8_t       pt1[16], ct[16], pt2[16], key[16], ctx[16];
  int           equ;
  
  hex2bin (ct, tv_ct);
  hex2bin (pt1, tv_pt);
  hex2bin (pt2, tv_pt);
  hex2bin (&ctx, tv_key);
    
  Noekeonx(&ctx, pt1, NOEKEON_ENCRYPT);
  equ = memcmp (pt1, ct, 16)==0;

  printf ("\nEncryption : %s : ",
      equ ? "OK" : "FAILED"); 
  print_bytes("CT", pt1, 16);
  
  Noekeonx(&ctx, pt1, NOEKEON_DECRYPT);
  equ = memcmp (pt1, pt2, 16)==0;
  
  printf ("\nDecryption : %s : ",
      equ ? "OK" : "FAILED"); 
  print_bytes("PT", pt1, 16);      
  return 0;
}
