


#include "groestl-ref.h"

void PrintHash(uint8_t *hashval, int hashbitlen) {
  int i;
  for (i = 0; i < (hashbitlen+7)/8; i++) 
    printf("%02x", hashval[i]);
  printf("\n");
}

int main(int argc, char *argv[])
{
  hashState ctx;
  uint8_t hashval[256];
  
  Init(&ctx, 256);
  Update(&ctx, argv[1], strlen(argv[1]));
  Final(&ctx, hashval);
  
  PrintHash(hashval, 256);
  
  return 0;
}
