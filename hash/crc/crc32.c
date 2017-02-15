
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC32    0xEDB88320

uint32_t xcrc32(
    uint32_t crc, 
    uint32_t inlen, 
    uint8_t *in)
{
  uint32_t i, c;
  
  crc = ~crc;
  
  while (inlen-- != 0)
  {
    crc ^= *in++;
    
    for (i=0; i<8; i++) {
      crc = (crc >> 1) ^ (MAGIC32 * (crc & 1));
    }    
    /*for (i=0; i<8; i++)
    {
      if (((crc ^ c) & 1) != 0)
        crc = (crc>>1) ^ MAGIC32;
      else
        crc = (crc>>1);
      c >>= 1;
    }*/
  }
  return ~crc;
}

uint32_t xcrc32x(uint32_t crc, uint32_t inlen, uint8_t *in);

#ifdef TEST
int main(int argc, char *argv[])
{
  if (argc!=2) {
    printf ("\ncrc32 <string>\n");
    return 0;
  }
  printf ("\ncrc32(\"%s\") = 0x%08X\n", 
      argv[1], 
      xcrc32x(0, strlen(argv[1]), argv[1]));
  return 0;
}
#endif 