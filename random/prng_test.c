
#include "prng.h"

uint32_t fpu_hash (char s[])
{
  size_t i;
  uint32_t r=0, y;
  char c;
  float x=pow (2, 32);
  size_t len=strlen(s);
  
  printf ("\n%-20s : ", s);
  
  for (i=0; i<len; i++) {
    c  = s[i];
    r ^= (uint32_t)(fabs(cos(c) * x));
    r = R(r, 1);
  }
  return r;
}

void prng_print (void *buf, size_t buflen)
{
  size_t i;
  uint8_t *out=(uint8_t*)buf;
  
  buflen=(buflen>sizeof(sponge)) ? sizeof(sponge) : buflen;
  
  printf ("\nPRN : ");
  for (i=0; i<buflen; i++)
  {
    printf ("%02x", out[i]);
  }
}

int main (void)
{
  uint8_t r[64];
  int i;
  
  // initialize buffer
  prng_update();
  
  for (i=0; i<64; i++) {
    // get random number
    memset (r, 0, sizeof (r));
    prng_squeeze (r, sizeof (r));
    prng_print (r, sizeof (r));
  }
  return 0;
}