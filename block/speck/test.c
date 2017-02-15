
#include <stdio.h>
#include <string.h>

#include "speck.h"

void print_bytes(char *s, void *p, int len) {
  int i;
  printf("%s : ", s);
  for (i=0; i<len; i++) {
    printf ("%02x ", ((uint8_t*)p)[i]);
  }
  putchar('\n');
}

// p = 0x3b7265747475432d 
uint8_t plain[]=
{ 0x74, 0x65, 0x72, 0x3b,
  0x2d, 0x43, 0x75, 0x74 };

// c = 0x8c6fa548454e028b  
uint8_t cipher[]=
{ 0x48, 0xa5, 0x6f, 0x8c, 
  0x8b, 0x02, 0x4e, 0x45 };

// key = 0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918   
uint8_t key[]=
{ 0x00, 0x01, 0x02, 0x03,
  0x08, 0x09, 0x0a, 0x0b,
  0x10, 0x11, 0x12, 0x13,
  0x18, 0x19, 0x1a, 0x1b };

int main (void)
{
  uint32_t subkeys[SPECK_RNDS*4], buf[8];
  int     equ;
  
  // copy plaintext to local buffer
  memcpy (buf, plain, 8);
  print_bytes("K ", key, 16);
  print_bytes("PT", plain, 8);
  print_bytes("CT", cipher, 8);
  
  // create sub keys
  speck_setkeyx (key, subkeys);
  // encrypt plain
  speck_encryptx (subkeys, SPECK_ENCRYPT, buf);
  // ok?
  equ = memcmp(cipher, buf, 8)==0;
  printf ("\nEncryption %s\n", equ ? "OK" : "FAILED");
  print_bytes("CT", buf, 8);
  
  // create sub keys (not really necessary for this example)
  speck_setkeyx (key, subkeys);
  // decrypt ciphertext
  speck_encryptx (subkeys, SPECK_DECRYPT, buf);
  // ok?
  equ = memcmp(plain, buf, 8)==0;
  printf ("\nDecryption %s\n", equ ? "OK" : "FAILED");
  print_bytes("PT", buf, 8);
  
  return 0;
}
