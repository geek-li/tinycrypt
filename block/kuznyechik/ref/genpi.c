#include <stdio.h>
#include <stdint.h>

// gcc -std=c11 genpi.c -o genpi && ./genpi

// multiplcation by matrix over GF(2)
uint8_t matmul(uint8_t a, uint8_t mat[8][8]) {
  uint8_t res = 0;
  int x, y;
  
  for (x = 0; x < 8; x++)
    if (a & (1 << (7 - x)))
      for (y = 0; y < 8; y++)
        res ^= mat[y][x] << (7 - y);
  return res;
}

// https://en.wikipedia.org/wiki/Finite_field_arithmetic
uint8_t fieldmul(uint8_t a, uint8_t b) {
  uint8_t p = 0;
  
  while (b) {
    if (b & 1) 
      p ^= a;
    if (a & 0x8)
      a = (a << 1) ^ 0x19;  // x^4 + x^3 + 1
    else
      a <<= 1;
    b >>= 1;
  }
  return p;
}

int main(int argc, char *argv[]) {
    int i, x, y, l, r, n, z;
    
uint8_t inv[16]   = 
{0x0,0x1,0xc,0x8,0x6,0xf,0x4,0xe,
 0x3,0xd,0xb,0xa,0x2,0x9,0x7,0x5};

uint8_t nu_0[16]  = 
{0x2,0x5,0x3,0xb,0x6,0x9,0xe,0xa,
 0x0,0x4,0xf,0x1,0x8,0xd,0xc,0x7};

uint8_t nu_1[16]  = 
{0x7,0x6,0xc,0x9,0x0,0xf,0x8,0x1,
 0x4,0x5,0xb,0xe,0xd,0x2,0x3,0xa};

uint8_t sigma[16] = 
{0xc,0xd,0x0,0x4,0x8,0xb,0xa,0xe,
 0x3,0x9,0x5,0x2,0xf,0x1,0x6,0x7};

uint8_t phi[16]   = 
{0xb,0x2,0xb,0x8,0xc,0x4,0x1,0xc,
 0x6,0x3,0x5,0x8,0xe,0x3,0x6,0xb};

uint8_t alpha[8][8] = {
    {0,0,0,0,1,0,0,0},
    {0,1,0,0,0,0,0,1},
    {0,1,0,0,0,0,1,1},
    {1,1,1,0,1,1,1,1},
    {1,0,0,0,1,0,1,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,1,1,0,1,0},
    {0,0,1,0,0,0,0,0},
};

uint8_t omega[8][8] = {
    {0,0,0,0,1,0,1,0},
    {0,0,0,0,0,1,0,0},
    {0,0,1,0,0,0,0,0},
    {1,0,0,1,1,0,1,0},
    {0,0,0,0,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {1,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,1},
};
  
    for(x = 0; x < 256; x++) {
        y = matmul(x, alpha);
        l = (y >> 4) & 0xf;
        r = y & 0xf;
        l = (r == 0) ? nu_0[l] : nu_1[fieldmul(l, inv[r])];
        r = sigma[fieldmul(r, phi[l])];
        y = matmul((l << 4) | r, omega);
        if ((x & 7)==0) putchar('\n');
        printf("0x%02x, ", y);
    }
    return 0;
}
