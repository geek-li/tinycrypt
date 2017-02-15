
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC32    0xEDB88320

// Castagnoli polynomial is 0x1edc6f41

uint32_t incrc32(uint32_t crc, uint32_t inlen, uint8_t *in)
{
  uint32_t i, c;
  
  while (inlen-- != 0)
  {
    c = *in++;
    
    for (i=0; i<8; i++)
    {
      if (((crc ^ c) & 1) != 0)
        crc = (crc>>1) ^ 0x82F63B78;
      else
        crc = (crc>>1);
      c >>= 1;
    }
  }
  return crc;
}
  
uint32_t intel(uint32_t crc, uint32_t inlen, uint8_t *in)
{
  __asm {
    pop    edi
    pop    esi
    pop    ebx
    pushad
    mov    edx, [esp+32+ 8]
    mov    ecx, [esp+32+12]
    mov    esi, [esp+32+16]    
    xor    eax, eax
crc32_api:
    lodsb
    crc32  edx, al
    loop   crc32_api
    mov    [esp+28], edx
    popad
    leave
    ret    
  }
}

uint32_t mcrc32(uint32_t x, uint8_t v)
{
  int      i;
  uint32_t crc = x;
  
  crc ^= (uint8_t)v;
    
  for (i=0; i<8; i++) {
    crc = (crc >> 1) ^ (0x82F63B78 * (crc & 1));
  }
  return crc;
}

uint32_t icrc32(uint32_t crc, uint32_t inlen, uint8_t *in)
{
  uint32_t i, c;
  
  while (inlen-- != 0) {
    c   = *in++;
    crc = mcrc32(crc, c);
  }
  return crc;
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
      incrc32(0, strlen(argv[1]) + 1, argv[1]));
 
  printf ("\ncrc32(\"%s\") = 0x%08X\n", 
      argv[1], 
      icrc32x(0, strlen(argv[1]) + 1, argv[1]));
      
  printf ("\nINTEL crc32(\"%s\") = 0x%08X\n", 
      argv[1], 
      intel(0, strlen(argv[1]) + 1, argv[1]));      
  return 0;
}
#endif 