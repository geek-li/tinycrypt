

// ChaCha20 test unit in C
// Odzhan

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cc20.h"

// 256-bit key
uint8_t key[32]=
{ 0xc4, 0x6e, 0xc1, 0xb1, 0x8c, 0xe8, 0xa8, 0x78,
  0x72, 0x5a, 0x37, 0xe7, 0x80, 0xdf, 0xb7, 0x35,
  0x1f, 0x68, 0xed, 0x2e, 0x19, 0x4c, 0x79, 0xfb,
  0xc6, 0xae, 0xbe, 0xe1, 0xa6, 0x67, 0x97, 0x5d };
  
// 64-bit nonce
uint8_t nonce[8]=
{ 0x1a, 0xda, 0x31, 0xd5, 0xcf, 0x68, 0x82, 0x21 };

// 512-bit input
uint8_t input[64]={ 0 };

// 512-bit output
uint8_t output[64]=
{ 0xf6, 0x3a, 0x89, 0xb7, 0x5c, 0x22, 0x71, 0xf9,
  0x36, 0x88, 0x16, 0x54, 0x2b, 0xa5, 0x2f, 0x06,
  0xed, 0x49, 0x24, 0x17, 0x92, 0x30, 0x2b, 0x00,
  0xb5, 0xe8, 0xf8, 0x0a, 0xe9, 0xa4, 0x73, 0xaf,
  0xc2, 0x5b, 0x21, 0x8f, 0x51, 0x9a, 0xf0, 0xfd,
  0xd4, 0x06, 0x36, 0x2e, 0x8d, 0x69, 0xde, 0x7f,
  0x54, 0xc6, 0x04, 0xa6, 0xe0, 0x0f, 0x35, 0x3f,
  0x11, 0x0f, 0x77, 0x1b, 0xdc, 0xa8, 0xab, 0x92 };

void dump_bytes (char *s, uint8_t b[], int len)
{
  int i;
  char c;
  
  printf ("\n%s=", s);
  for (i=0; i<len; i++) {
    if ((i & 7)==0) putchar('\n');
    if (i==len-1) c='\n'; else c=',';
    printf ("0x%02x%c ", b[i], c);
  }
}
int main (void) {
  cc20_ctx ctx;
  uint8_t    pt[64]={0};
  uint8_t    r, i;
  
  // setup 256-bit key
  cc20_setkey (&ctx, key, nonce);
  
  printf ("\ncounter = %08x %08x\n", 
    ctx.s.v32[13], ctx.s.v32[12]);
    
  for (i=0; i<10; i+= 2) {
    printf ("state[%02i - %02i] = 0x%08x 0x%08x\n", 
      i, i+1, ctx.s.v32[i], ctx.s.v32[i+1]);
  }
  
  printf ("\nencrypting block\n");
  cc20_encrypt (&ctx, input, 64);
 
  printf ("\ncounter = %08x %08x\n", 
    ctx.s.v32[13], ctx.s.v32[12]);
    
  for (i=0; i<10; i+= 2) {
    printf ("state[%02i - %02i] = 0x%08x 0x%08x\n", 
      i, i+1, ctx.s.v32[i], ctx.s.v32[i+1]);
  }
  
  r=memcmp (input, output, 64)==0;
  printf ("\nEncryption test %s", r ? "passed":"failed");
  
  dump_bytes ("ciphertext", input, 64);
  
  // do 20 rounds of decryption
  cc20_setkey (&ctx, key, nonce);
  cc20_encrypt (&ctx, input, 64);
  
  r=memcmp (input, pt, 64)==0;
  printf ("\nDecryption test %s", r ? "passed":"failed");
  
  dump_bytes ("plaintext", input, 64);
  
  return 0;
}
