#include <iostream>
#include <cstdint>
#include <intrin.h>

typedef union _vector_t { 
  uint8_t   v8[4];
  uint16_t v16[2];
  uint32_t v32;
  uint64_t v64;
} vector;

// Maximum Distance Separable code
// Twofish uses a single 4-by-4 MDS matrix over GF(2**8).
// Twofish uses 1,91,239 in a non-circulant matrix.
uint8_t matrix[4][4] = 
{ { 0x01, 0xEF, 0x5B, 0x5B },
  { 0x5B, 0xEF, 0xEF, 0x01 },
  { 0xEF, 0x5B, 0x01, 0xEF },
  { 0xEF, 0x01, 0xEF, 0x5B } };

uint32_t mds(uint32_t w)
{
  vector   x;
  int i;
  uint32_t j, x0, y;
  vector acc;
  
  x.v32 = w;
  acc.v32 = 0;
  
  for (i=0; i<4; i++) 
  {
    for (j=0; j<4; j++) 
    {
      x0 = matrix[i][j];
      y  = x.v8[j];
      while (y)
      {
        if (x0 > (x0 ^ 0x169))
          x0 ^= 0x169;
        if (y & 1)
          acc.v8[i] ^= x0;
        x0 <<= 1;
        y >>= 1;
      }
    }
  }
  return acc.v32;
}

int main()
{
  int b[4];

  std::cout << std::hex << mds (fabs(sqrt((double)__rdtsc()) * pow(2.0,32))) << std::endl;
  std::cout << std::hex << mds (fabs(sin((double)__rdtsc()) * pow(2.0,32))) << std::endl;
  std::cout << std::hex << mds (fabs(cos((double)__rdtsc()) * pow(2.0,32))) << std::endl;
  
  for (uint32_t a = 0; a < -1; a++)
  {
    __cpuid(b, a);
    if (b[0]!=1)
    {
      std::cout << "The code " << 
    a << " gives " << b[0] << 
    ", " << b[1] << ", " << b[2] << ", " << b[3] << std::endl;
    }
  }

  return 0;
}
