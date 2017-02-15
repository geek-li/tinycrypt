
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC16    0xA001

uint16_t xcrc16(
    uint16_t crc, 
    uint16_t inlen, 
    uint8_t *in)
{
  uint32_t i;
  
  crc = ~crc;
  
  while (inlen-- != 0)
  {
    crc ^= *in++;
    
    for (i=0; i<8; i++) {
      crc = (crc >> 1) ^ (MAGIC16 * (crc & 1));
    } 
  }
  return ~crc;
}

uint32_t xcrc32x(uint32_t crc, uint32_t inlen, uint8_t *in);

#ifdef TEST
int main(int argc, char *argv[])
{
  if (argc!=2) {
    printf ("\ncrc16 <string>\n");
    return 0;
  }
  printf ("\ncrc16(\"%s\") = 0x%04X\n", 
      argv[1], 
      xcrc16(0, strlen(argv[1]), argv[1]));
  return 0;
}
#endif 