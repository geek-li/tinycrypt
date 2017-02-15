
// not working, don't use
//
// do all 64-bit blocks
// do length << 56 + anything in mod 64-bits

#include "siphash.h"

void perm(int last, uint64_t x, uint64_t* v)
{
  int      rnds;
  uint64_t v0, v1, v2, v3;
  
  v0=v[0];
  v1=v[1];
  v2=v[2];
  v3=v[3];
  
  if (last) {
    v2 ^= 0xff;
  } else {
    v3 ^= x;
  }
  // if it's last round, do 4 loops
  rnds = 2 << last;
  
  do {
    v0 += v1;
    v1 = ROTL64(v1, 13);
    v1 ^= v0;
    v0 = ROTL64(v0, 32);

    v2 += v3;
    v3 = ROTL64(v3, 16);
    v3 ^= v2;

    v2 += v1;
    v1 = ROTL64(v1, 17);
    v1 ^= v2;
    v2 = ROTL64(v2, 32);

    v0 += v3;
    v3 = ROTL64(v3, 21);
    v3 ^= v0;
  } while (--rnds);
  
  if (!last) {
    v0 ^= x;
  }
  v[0]=v0;
  v[1]=v1;
  v[2]=v2;
  v[3]=v3;
}

void siphash24x(void *out, void const *in, 
  int inlen, void const *key)
{
  uint64_t v[4];
  uint64_t k0, k1, x, b=inlen;
  int      c, r, i, len=inlen;
  uint8_t  *p=(uint8_t*)in;
  
  b <<= 56;
  
  k0 = ((uint64_t*)key)[0];
  k1 = ((uint64_t*)key)[1];
  
  v[0] = k0 ^ 0x736F6D6570736575ULL;
  v[1] = k1 ^ 0x646F72616E646F6DULL;
  v[2] = k0 ^ 0x6C7967656E657261ULL;
  v[3] = k1 ^ 0x7465646279746573ULL;

  // process all bytes
  while (len >= 0)
  {
    // get 8 or whatever bytes remaining.
    x = 0;
    r = (len < 8) ? len : 8;
    // if not zero, load bytes
    if (len!=0)
    {
      for (i=0; i<r; i++) {
        x |= *p++;
        x  = ROTR64(x, 8);
      }
      // subtract r from len
      len -= r;
      // if r is odd, then append b
    }
    if (r<8) {
      x = ROTL64(x, (r << 3));
      x |= b;
    }
    // permutate
    perm(0, x, v);
    // if more left, continue
    if (len>0) continue;
    // if less than 8, don't permutate last block
    if (r<8) break;
    perm(0, b, v);
    break;
  }
  // do last permutation
  perm(1, x, v);
  
  ((uint64_t*)out)[0] = v[0] ^ v[1] ^ v[2] ^ v[3];
}

