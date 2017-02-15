


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t hsh32(const void *in, const size_t inlen, const void *key);
int halfsiphash(const uint8_t *in, const size_t inlen, const uint8_t *k,
        uint8_t *out, const size_t outlen);
 
int sh32x(
  const uint8_t *data, 
  const size_t inlen, 
  const uint8_t *key);

int shp(
  const uint8_t *data, 
  const size_t inlen, 
  const uint8_t *key);

int shx(
  const uint8_t *data, 
  const size_t inlen, 
  const uint8_t *key);
  
// 92EA1052 = "", 01234567
// 435353F5 = "here we go again" 01234567
   
int main (int argc, char *argv[])
{
  uint32_t h;
  uint8_t sh[8];
  
  if (argc!=3) {
    printf ("\nhsh32 <string> <key>\n");
    return 0;
  }
  
  h = hsh32x(argv[1], strlen(argv[1]), argv[2]);

  printf("\n");
    
  halfsiphash(argv[1], strlen(argv[1]), argv[2], sh, 32);
  
  printf ("\n%08X = %08X | 92EA1052 or 435353F5", h, ((uint32_t*)sh)[0]);
  
  return 0;
}
