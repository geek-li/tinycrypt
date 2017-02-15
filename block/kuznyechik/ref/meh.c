#include <stdio.h>
#include <stdint.h>

// gcc -std=c11 genpi.c -o genpi && ./genpi

// multiplication by matrix over GF(2)
uint8_t matmul(uint8_t a, uint8_t mat[8][8]) {
  uint8_t res = 0;
  uint8_t x, y, z;
  
  for (x = 0; x < 8; x++) 
  {
    if (a & (1 << (7 - x)))
    {
      for (y = 0; y < 8; y++)
      {
        z = mat[y][x] << (7 - y);
        res ^= z;
      }
    }
  }
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

// convert 8 bytes to 16
void b2w(void *out, void *in) {
  uint8_t i, x;
  
  for (i=0; i<16; ) {
    x=((uint8_t*)in)[i>>1];
    
    ((uint8_t*)out)[i++] = x & 15;
    ((uint8_t*)out)[i++] = x >> 4;
  }
}

uint8_t invx[8]   = {0x10,0x8c,0xf6,0xe4,0xd3,0xab,0x92,0x57};
uint8_t nu_0x[8]  = {0x52,0xb3,0x96,0xae,0x40,0x1f,0xd8,0x7c};
uint8_t nu_1x[8]  = {0x67,0x9c,0xf0,0x18,0x54,0xeb,0x2d,0xa3};
uint8_t sigmax[8] = {0xdc,0x40,0xb8,0xea,0x93,0x25,0x1f,0x76};
uint8_t phix[8]   = {0x2b,0x8b,0x4c,0xc1,0x36,0x85,0x3e,0xb6};
//uint8_t alphax[8] = {0x08,0x41,0x43,0xef,0x8a,0x44,0x1a,0x20};
uint8_t alphax[8] = {0x18,0x74,0x11,0x02,0x9a,0x14,0x3a,0x70};

uint8_t omegax[8] = {0x0a,0x04,0x20,0x9a,0x08,0x44,0x82,0x01};

/*
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
*/
int main(int argc, char *argv[]) {
    int i, x, y, l, r, n, z;
    uint8_t pi[256];
    uint8_t *p;
    uint8_t inv[16], nu_0[16], nu_1[16], sigma[16], phi[16], alpha[8][8], omega[8][8];

    for (x=0; x<8; x++) {
      //putchar('\n');
      r=0;
      for (y=0; y<8; y++) {
        r *= 2;
        r += alpha[y][x];
      }
      //printf ("0x%02x, ", r);
    }
    //return 0;

    b2w(inv, invx);
    b2w(nu_0, nu_0x);
    b2w(nu_1, nu_1x);
    b2w(sigma, sigmax);
    b2w(phi, phix);
    
    for(x = 0; x < 256; x++) {
        y = matmul(x, alpha);
        l = (y >> 4) & 0xf;
        r = y & 0xf;
        l = (r == 0) ? nu_0[l] : nu_1[fieldmul(l, inv[r])];
        r = sigma[fieldmul(r, phi[l])];
        y = matmul((l << 4) | r, omega);
        pi[x] = y;
        //if ((x & 7)==0) putchar('\n');
        //printf("%d, ", y);
    }
    return 0;
}
