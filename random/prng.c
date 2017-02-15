

// PRNG using FPU functions for numbers not exceeding 512-bits
// Odzhan

#include "prng.h"

uint32_t gf_mul2 (uint32_t w) {
  uint32_t t = w & 0x80808080;
  
  return ( (w ^ t ) << 1) ^ ( ( t >> 7) * 0x0000001B);
}

// ------------------------------------
// mix columns in state
// ------------------------------------
uint32_t MixColumn (uint32_t w) {
  return ROTR32(w, 8) ^ 
         ROTR32(w, 16) ^ 
         ROTR32(w, 24) ^ 
         gf_mul2(ROTR32(w, 8) ^ w);
}

void MixColumns (void)
{
  uint32_t i, t, w;

  for (i=0; i<16; i++)
  {
    w = sponge[i];
    sponge[i] = MixColumn(w);
  }
}

void ShiftRows (void)
{
  uint32_t x, i, j;

  // shift 4 rows
  for (i=0; i<4; i++) {
    x=0;
    // get row
    for (j=i; j<16; j+=4) {
      //x |= state->v8[j];
      x=ROTR32(x, 8);
    }
    x = ROTR32(x, i*8);
    // set row
    for (j=i; j<16; j+=4) {
      //state->v8[j] = (x & 0xff);
      x >>= 8;
    }
  }
}

// salsa function by Bernstein
void prng_shuffle (uint32_t s[])
{
  uint32_t x[16];
  size_t i;

  for (i=0; i<16; i++) 
    x[i] = s[i];
  
  for (i = 0; i < 8; i += 2) {
    /* Operate on columns. */
    x[ 4] ^= R(x[ 0]+x[12], 7);  x[ 8] ^= R(x[ 4]+x[ 0], 9);
    x[12] ^= R(x[ 8]+x[ 4],13);  x[ 0] ^= R(x[12]+x[ 8],18);

    x[ 9] ^= R(x[ 5]+x[ 1], 7);  x[13] ^= R(x[ 9]+x[ 5], 9);
    x[ 1] ^= R(x[13]+x[ 9],13);  x[ 5] ^= R(x[ 1]+x[13],18);

    x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] ^= R(x[14]+x[10], 9);
    x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],18);

    x[ 3] ^= R(x[15]+x[11], 7);  x[ 7] ^= R(x[ 3]+x[15], 9);
    x[11] ^= R(x[ 7]+x[ 3],13);  x[15] ^= R(x[11]+x[ 7],18);

    /* Operate on rows. */
    x[ 1] ^= R(x[ 0]+x[ 3], 7);  x[ 2] ^= R(x[ 1]+x[ 0], 9);
    x[ 3] ^= R(x[ 2]+x[ 1],13);  x[ 0] ^= R(x[ 3]+x[ 2],18);

    x[ 6] ^= R(x[ 5]+x[ 4], 7);  x[ 7] ^= R(x[ 6]+x[ 5], 9);
    x[ 4] ^= R(x[ 7]+x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],18);

    x[11] ^= R(x[10]+x[ 9], 7);  x[ 8] ^= R(x[11]+x[10], 9);
    x[ 9] ^= R(x[ 8]+x[11],13);  x[10] ^= R(x[ 9]+x[ 8],18);

    x[12] ^= R(x[15]+x[14], 7);  x[13] ^= R(x[12]+x[15], 9);
    x[14] ^= R(x[13]+x[12],13);  x[15] ^= R(x[14]+x[13],18);
  }
  for (i=0; i<16; i++)
    s[i] += x[i];
}

// Square a 32-bit unsigned integer to
// obtain the 64-bit result and return
// the upper 32 bits XOR the lower 32 bits
#ifndef _WIN64
__declspec(naked) uint32_t RABBIT_g_func(uint32_t x)
#else
uint32_t RABBIT_g_func(uint32_t x)
#endif
{
  uint32_t h;
   #ifndef _WIN64
   __asm {
     mov    eax, [esp+4]
     xor    edx, edx
     mul    eax
     xor    eax, edx
     ret
   };
   #elif _WIN64
   uint64_t sum = U64V(x) * x;

   h = U32V((sum >> 32) ^ (sum & 0xFFFFFFFF));
   return h;
   #else
   uint32_t a, b, l;
   // Construct high and low argument for squaring
   a = x & 0xFFFF;
   b = x >> 16;

   // Calculate high and low result of squaring
   h = (((U32V(a * a) >> 17) + U32V(a * b)) >> 15) + b * b;
   l = x * x;

   h ^= l;
   // Return high XOR low
   return h;
   #endif
}

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

uint32_t Mod(uint32_t c)
{
  uint32_t c1, c2;
  
  c2=(c<<1) ^ ((c & 0x80) ? 0x14d : 0);
  c1=c2 ^ (c>>1) ^ ((c & 1) ? (0x14d>>1) : 0);

  return c | (c1 << 8) | (c2 << 16) | (c1 << 24);
}

// compute RS(12,8) code with the above polynomial as generator
// this is equivalent to multiplying by the RS matrix
uint32_t reedsolomon(uint64_t x)
{
  uint32_t i, low, high;
    
  low  = SWAP32(x & 0xFFFFFFFF);
  high = x >> 32;
  
  for (i=0; i<8; i++)
  {
    high = Mod(high >> 24) ^ (high << 8) ^ (low & 255);
    low >>= 8;
  }
  return high;
}

// calculate cube root
uint32_t cbrt (uint32_t x)
{
  return (uint32_t)(fabs(pow(fabs(x),1.0/3.0) * pow(2, 32)));
}

// MD4 G function
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))

// using cosine, square root and tangent
uint32_t r1 (uint32_t x) {
  uint32_t b, c, d;
  
  b  = (uint32_t)(fabs(cos(x)  * pow(2,32)));
  c  = (uint32_t)(fabs(sqrt(x) * pow(2,32)));
  d  = (uint32_t)(fabs(tan(x)  * pow(2,32)));
  
  return G(b, c, d);
}

// using sine, arc tangent and logarithm
uint32_t r2 (uint32_t x) {
  uint32_t b, c, d;
  
  b  = (uint32_t)(fabs(sin(x)  * pow(2,32)));
  c  = (uint32_t)(fabs(atan(x) * pow(2,32)));
  d  = (uint32_t)(fabs(log(x)  * pow(2,32)));
  
  return G(b, c, d);
}

// initialize buffer using CPU time stamp counter
void prng_init (void)
{
  size_t   i;
  uint32_t r, s, ts;
  
  for (i=0; i<16; i++) {
    // part 1
    //ts = mds(__rdtsc());
    
    // part 2
    ts = mds(RABBIT_g_func(__rdtsc()));
    sponge[i]=ts;
  }
  //prng_shuffle();
}

void prng_absorb (void* in, size_t inlen)
{
  uint32_t x, r, s, *p=(uint32_t*)in;
  size_t i;
  
  for (i=0; i<inlen/4; i++) {
    x=__rdtsc();
    r=mds(p[i] * x);
    sponge[i]=mds(p[i] + r);
  }
}

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR32(x,2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define EP1(x) (ROTR32(x,6) ^ ROTR32(x,11) ^ ROTR32(x,25))
#define SIG0(x) (ROTR32(x,7) ^ ROTR32(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTR32(x,17) ^ ROTR32(x,19) ^ ((x) >> 10))

void prng_update(void)
{
  size_t   i;
  uint32_t r, s, ts, t1, t2;
  
  for (i=0; i<128; i++) {
    sponge[i] ^= G(mds(__rdtsc()), RABBIT_g_func(__rdtsc()), reedsolomon(__rdtsc()));
  }
  
  /*for (i=16; i<128; i++) {
    sponge[i] = SIG1(sponge[i-2]) + sponge[i-7] + SIG0(sponge[i-15]) + sponge[i-16];
  }
  for (i=0; i<128; i++) {
    t1 = sponge[7] + EP1(sponge[4]) + CH(sponge[4], sponge[5], sponge[6]) + sponge[i];
    t2 = EP0(sponge[0]) + MAJ(sponge[0], sponge[1], sponge[2]);
    sponge[i] ^= (t1 * t2);
  }*/
  for (i=0; i<128/16; i++) {
    prng_shuffle(&sponge[i*16]);
  }
  //MixColumns();
  //ShiftRows();
}

void prng_squeeze (void* out, size_t outlen)
{
  uint8_t *s=(uint8_t*)sponge;
  size_t i;
  
  // random number can't exceed 4096-bits
  outlen=(outlen>sizeof(sponge)) ? sizeof(sponge) : outlen;
  
  memcpy (out, sponge, outlen);
  prng_update();
}

