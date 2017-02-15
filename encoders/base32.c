

#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <string.h>

#define U8V(v)  ((uint8_t)(v)  & 0xFFU)
#define U16V(v) ((uint16_t)(v) & 0xFFFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFULL)

#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))

#define ROTR8(v, n) ROTL8(v, 8 - (n))
#define ROTR16(v, n) ROTL16(v, 16 - (n))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define ROTR64(v, n) ROTL64(v, 64 - (n))

#define SWAP16(v) \
  ROTL16(v, 8)

#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFUL) | \
   (ROTL32(v, 24) & 0xFF00FF00UL))

#define SWAP64(v) \
  ((ROTL64(v,  8) & 0x000000FF000000FFULL) | \
   (ROTL64(v, 24) & 0x0000FF000000FF00ULL) | \
   (ROTL64(v, 40) & 0x00FF000000FF0000ULL) | \
   (ROTL64(v, 56) & 0xFF000000FF000000ULL))
   
char buf[1024];

char* b32_encode(char str[], int inlen) 
{
  int      i, j, k, pad=8;
  uint64_t x, z;
  uint8_t  *in=(uint8_t*)str;
  uint8_t  *p=buf;
  uint8_t  c;

  for (i=0; i<inlen;)
  {
    z = x = 0;
    // read 5 bytes
    for (j=0; j<5 && i < inlen; i++, j++) {
      x <<= 8;
      x |= in[i];
    }
    // if less than 5
    // shift left according to how many bytes read
    // calculate how many characters will be printed
    if (j<5) {
      x <<= j*2; 
      if (j==1) pad=2;
      if (j==2) pad=4;
      if (j==3) pad=5; 
      if (j==4) pad=7; 
    }
    // split into 5-bit blocks
    for (k=0; k<pad; k++) {
      z = ROTL64(z, 5);
      z |= (x & 31);
      x >>= 5;
    }
    // create encoded string
    for (j=0; j<8; j++) {
      c = '=';
      if (j < pad) {        
        c = (z & 31);
        if (c <= ('Z' - 'A')) c += 'A';
        else c += 24;
      }
      *p++ = c;
      z >>= 5;
    }    
  }
  *p = 0;
  return buf;
}

int main(int argc, char *argv[])
{
  int     inlen, i;
  /*
  uint8_t box[256];
  
  for (i=0; i<256; i++) {
    box[i] = (uint8_t)i;
  }
  
  printf ("%s", b32_encode(box, 256));
  return 0;*/
  
  if (argc != 2) {
    printf ("\nusage: base32 <string>");
    return 0;
  }
  
  inlen = strlen(argv[1]);
  
  printf ("BASE32(\"%s\") = \"%s\"", 
      argv[1], 
      b32_encode(argv[1], inlen));
  return 0;
}
  