
// not working, don't use

#include "siphash.h"

static void sipround(uint64_t* v0, uint64_t* v1, 
  uint64_t* v2, uint64_t* v3)
{
  *v0 += *v1;
  *v1 = ROTL64(*v1, 13);
  *v1 ^= *v0;
  *v0 = ROTL64(*v0, 32);

  *v2 += *v3;
  *v3 = ROTL64(*v3, 16);
  *v3 ^= *v2;

  *v2 += *v1;
  *v1 = ROTL64(*v1, 17);
  *v1 ^= *v2;
  *v2 = ROTL64(*v2, 32);

  *v0 += *v3;
  *v3 = ROTL64(*v3, 21);
  *v3 ^= *v0;
}

void siphash24(uint8_t* out, void const* in, int inlen, uint8_t const* key)
{
  uint64_t k0, k1, v0, v1, v2, v3, x, b=inlen;
  int c, r, i, len=inlen;
  uint8_t *p=(uint8_t*)in;
  
  b <<= 56;
  
  k0=((uint64_t*)key)[0];
  k1=((uint64_t*)key)[1];
  
  v0 = k0 ^ 0x736F6D6570736575ULL;
  v1 = k1 ^ 0x646F72616E646F6DULL;
  v2 = k0 ^ 0x6C7967656E657261ULL;
  v3 = k1 ^ 0x7465646279746573ULL;

  while (len>=0)
  {
    // get bytes from buffer.
    x=0;
    r=(len<8) ? len : 8;
    for (i=0; i<r; i++) {
      x |= *p++;
      x  = ROTR64(x, 8);
    }
    // last block is uneven?
    if (r & 7) 
    {
      x = ROTL64(x, ((r & 7) << 3));
      x |= b;
      printf ("\nI've got %016llX", x);
    }
rnd_loop:
    // subtract number of bytes in x from total len
    len -= r;
    // call sipround 2 times
    c=2;
    // if length is less than zero, this is last call
    if (len<0) 
    { 
      // so xor v2 by 255 and call sipround 4 times
      v2 ^= 0xFF; 
      c=4; 
      printf ("\nI've got b=%016llX and v0=%llX v1=%llX v2=%llX v3=%llX", b, v0, v1, v2, v3);
    } else {
      // else xor v3 by bytes in x
      v3  ^= x;
    }
    // call sipround 2 or 4 times depending on c
    for (i=0; i<c; i++) {
      sipround(&v0, &v1, &v2, &v3);
    }
    // if c is 2, xor v0 by bytes in x
    if (c==2) {
      v0 ^= x;
    }
    // all blocks processed?
    if (len == 0) {
      printf ("\nok");
      x=b;
      goto rnd_loop;
    }
  }
  ((uint64_t*)out)[0] = v0 ^ v1 ^ v2 ^ v3;
}

