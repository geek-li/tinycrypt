/**
  Copyright © 2016 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include "binmath.h"

uint64_t rseed;
uint32_t ra, rb, rc, rd;

// convert decimal string to 64-bit integer.
// max is 18446744073709551615
uint64_t dec2bin (char dec[])
{
  uint64_t r=0;
  char *s;
  
  for (s=dec; *s; s++) {
    if (*s >= '0' && *s <= '9') {
      r *= 10;
      r += *s - '0';
    }
  }
  return r;
}

// convert 64-bit integer to decimal string
char *bin2dec (uint64_t x)
{
  static char r[256], s[256];
  uint64_t t, v;
  char *p;
  
  // divide integer by 16 after each modulo
  for (t=x, v=0; t != 0 && v < 255; t /= 10) {
    s[v++] = (t % 10) + '0';
  }
  // reverse string
  p=r;
  while (v--) {
    *p++ = s[v];
  }
  *p=0;
  return r;
}

// convert 64-bit integer to hexadecimal string
char *bin2hex (uint64_t x)
{
  static char r[256], s[256];
  uint64_t t, u, v;
  char *p;
  
  // divide integer by 16 after each modulo
  for (t=x, v=0; t != 0 && v < 255; t >>= 4) {
    u = t % 16;
    s[v++] = (char)((u < 10) ? (u + '0') : (u + '7'));
  }
  // reverse string
  p=r;
  while (v--) {
    *p++ = s[v];
  }
  *p=0;
  return r;
}

// convert hexadecimal string to 64-bit integer
// max is FFFFFFFFFFFFFFFF
uint64_t hex2bin (char hex[])
{
  uint64_t r=0;
  char *p;
  char c;
  
  for (p=hex; *p; p++) {
    c=*p;
    r *= 16;
    if (c >= '0' && c <= '9') { 
      c = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      c = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      c = c - 'A' + 10;
    }
    r += c;
  }
  return r;
}

// get number of bits
uint64_t getbits (uint64_t x)
{
  uint32_t n=0;
  while (x) {
    x >>= 1;
    n++;
  }
  return n;
}

// add y to x
uint64_t add (uint64_t x, uint64_t y)
{
  uint64_t a=x, b=y, c=0;
  
  while (b > 0) {
    c = a & b;  
    a = a ^ b;
    b = c << 1;
  }
  return a;
}

// subtract y from x
uint64_t subtract (uint64_t x, uint64_t y)
{
  uint64_t a=x, b=y, c;
  a = ~a;
  
  while (b != 0) {
    c = a & b;  
    a = a ^ b; 
    b = c << 1;
  }
  return ~a;
}

// Multiply x by y
uint64_t multiply (uint64_t x, uint64_t y) 
{
  uint64_t a=x, b=y, r=0;
  
  while (a > 0) {
    if (a & 1) {
      r = add (r, b);
    }
    b = add(b, b);
    a >>= 1;
  }  
  return r;
}

// return x Squared
uint64_t square (uint64_t x) {
  return multiply (x, x);
}

// raise b to the power of e
uint64_t power (uint64_t b, uint64_t e) {
  uint64_t r = 1;

  while (e > 0) {
    if (e & 1) {
      r = multiply (r, b);
    }
    b = square (b);
    e >>= 1;
  }
  return r;
}

// return x ^ 3
// x shouldn't exceed 2642245 in decimal
uint64_t cube (uint64_t x) {
  return multiply (x, square(x));
}

// Modulo operation
uint64_t modulo (uint64_t dd, uint64_t dr)
{
  uint64_t r = 0;
  int i, bits, cf;
  
  bits=getbits(dd);

  for (i=bits-1; i>=0; i--)
  {
    // set cf if bit set
    cf=(dd & ((uint64_t)1 << i)) ? 1 : 0;
    
    r = add(add(r, r), cf);
    
    // if r exceeds divisor
    if (r >= dr) {
      // subtract divisor
      r = subtract (r, dr); 
    }
  }
  return r;
}

// Division
void divide (uint64_t *q_out, uint64_t *r_out, 
  uint64_t dd, uint64_t dr) {
  
  uint64_t r=0, q=0;
  int i, bits, cf;
  
  bits=getbits(dd);

  for (i=bits-1; i>=0; i--)
  {
    // add instead of shift
    q = add(q, q);
    
    // set cf if bit set
    cf=(dd & ((uint64_t)1 << i)) ? 1 : 0;
    
    r = add(add(r, r), cf);
    
    // if r exceeds divisor
    if (r >= dr) {
      // subtract divisor
      r = subtract (r, dr);
      // add 1
      q |= 1; 
    }
  }
  *q_out = q;
  *r_out = r;
}

// Modular Addition
uint64_t addmod (uint64_t a, uint64_t b, uint64_t m)
{
  return modulo (add (a, b), m);
}

// Modular Subtraction
uint64_t submod (uint64_t a, uint64_t b, uint64_t c)
{
  uint64_t d = c - b;
  return addmod (a, d, c);
}

// Modular Division
uint64_t divmod (uint64_t a, uint64_t b, uint64_t m)
{
  uint64_t r, q;
  
  divide (&q, &r, a, b);
  return modulo (q, m);
}

// Modular multiplication
uint64_t mulmod (uint64_t b, uint64_t e, uint64_t m)
{
  uint64_t r = 0, t = b;
  
  while (e > 0)
  {
    if (e & 1) {
      r = modulo (add (r, t), m);
    }
    t = modulo (add(t, t), m);
    e >>= 1;
  }
  return r;
}

// Modular exponentiation
uint64_t powmod (uint64_t a, uint64_t b, uint64_t m)
{
  uint64_t r = 1;

  while (b > 0) {
    if (b & 1) {
      r = mulmod (r, a, m);
    }
    a = mulmod (a, a, m);
    b >>= 1;
  }
  return r;
}

// Modular squaring
uint64_t sqrmod (uint64_t a, uint64_t p)
{
  return mulmod (a, a, p);
}

// Euler's Totient function
// For RSA algorithm calculating PHI is faster 
// with multiply (p-1, q-1)
uint64_t totient (uint64_t n)
{
  uint64_t i, ans = n;
  uint64_t q, r;

  for (i=2; ; i++) {
    // i * i <= n
    if (multiply (i, i) > n) break;
    
    // if (n % i == 0)
    if (modulo (n, i) == 0) {
      // ans -= ans / i
      divide (&q, &r, ans, i);
      ans = subtract (ans, q);
    }
    
    for (;;) {
      // while (n % i == 0)
      divide (&q, &r, n, i);
      if (r != 0) break;
      n = q;
    }
  }
  if (n != 1) {
    divide (&q, &r, ans, n);
    ans = subtract (ans, q);
  }
  return ans;   
}

// modular inversion
uint64_t invmod (uint64_t a, uint64_t m)
{
  uint64_t j = 1, i = 0, b = m, c = a, x, y;
  
  while (c != 0)
  {
    // x = b / c;
    // y = b - x * c;
    divide (&x, &y, b, c);
    b = c; 
    c = y;
    y = j;
    // j = i - j * x;
    j = subtract (i, multiply (j, x));
    i = y;
  }
  if ((int64_t)i < 0) {
    // i += m;
    i = add (i, m);
  }
  return i;
}

// generate 32-bit hash
// would normally use cryptographic hash
uint32_t hash (char s[])
{
  uint32_t h=0x12345678;
  size_t i, len=strlen (s);
  
  for (i=0; i<len; i++)
  {
    h += s[i];
    h = ((h << 5) | (h >> (32-5)));
  }
  return h;
}

// seed rng
void srandom (uint64_t n) {
  rseed = n;
  ra = 0x67452301;
  rb = 0xefcdab89;
  rc = 0x98badcfe;
  rd = 0x10325476;
}

void salsa20_8 (uint32_t B[16])
{
  uint32_t x[16];
  size_t i;

  for (i=0; i<16; i++) x[i] = B[i];
  
  for (i = 0; i < 8; i += 2) {
#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
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
#undef R
  }
  for (i = 0; i < 16; i++)
  B[i] += x[i];
}

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#define ROL32(a, n)(((a) << (n)) | (((a) & 0xffffffff) >> (32 - (n))))
#define ROR32(a, n)((((a) & 0xffffffff) >> (n)) | ((a) << (32 - (n))))

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define FF(a, b, c, d, x, s) { \
    (a) += F ((b), (c), (d)) + (x); \
    (a) = ROL32 ((a), (s)); \
  }
#define GG(a, b, c, d, x, s) { \
    (a) += G ((b), (c), (d)) + (x) + 0x5a827999L; \
    (a) = ROL32 ((a), (s)); \
  }
#define HH(a, b, c, d, x, s) { \
    (a) += H ((b), (c), (d)) + (x) + 0x6ed9eba1L; \
    (a) = ROL32 ((a), (s)); \
  }
#define S11 3
#define S12 7
#define S13 11
#define S14 19

#define S21 3
#define S22 5
#define S23 9
#define S24 13

#define S31 3
#define S32 9
#define S33 11
#define S34 15

uint64_t squeeze (uint64_t n) 
{
  uint64_t i, r=0;
  uint32_t x[16];
  uint8_t *p=(uint8_t*)x;
  
  x[0] *= (rseed >> 32);
  x[1] *= (rseed & 0xFFFFFFFF);
  x[2] += (rseed >> 32);
  x[3] += (rseed & 0xFFFFFFFF);
  
  for (i=4; i<16; i++) {
    x[i] = (ROL32(x[i-4], 1) ^ 
            ROL32(x[i-3], 3) ^ 
            ROL32(x[i-2], 5) ^ 
            ROL32(x[i-1], 7));
  }

  //salsa20_8 (x);

  FF(ra, rb, rc, rd, x[ 0], S11); 
  FF(rd, ra, rb, rc, x[ 1], S12); 
  FF(rc, rd, ra, rb, x[ 2], S13); 
  FF(rb, rc, rd, ra, x[ 3], S14); 

  FF(ra, rb, rc, rd, x[ 4], S11); 
  FF(rd, ra, rb, rc, x[ 5], S12); 
  FF(rc, rd, ra, rb, x[ 6], S13); 
  FF(rb, rc, rd, ra, x[ 7], S14); 
  
  // ----------------------------

  GG(ra, rb, rc, rd, x[ 8], S21); 
  GG(rd, ra, rb, rc, x[ 9], S22); 
  GG(rc, rd, ra, rb, x[10], S23); 
  GG(rb, rc, rd, ra, x[11], S24); 

  GG(ra, rb, rc, rd, x[12], S21); 
  GG(rd, ra, rb, rc, x[13], S22); 
  GG(rc, rd, ra, rb, x[14], S23); 
  GG(rb, rc, rd, ra, x[15], S24);
  
  x[0] += ra;
  x[1] += rb;
  x[2] += rc;
  x[3] += rd;
  
  for (i=0; i<n/8;) {
    if (*p == 0) p++;
    r <<= 8;
    r |= *p++;
    i++;
  }
  //printf ("\nr is %llu-bits %llX", n, r);
  return r;
}

// generate "random" integer
uint64_t random (uint64_t n) {
  uint64_t i, h;
  uint64_t x=0;
  if (rseed==0) {
    srandom (__rdtsc() | time(0));  // not recommended but only for demo purposes
  }
  n = (n + 8) & -8;
  if (n>64) n=64;
  if (n==0) n=8;
  rseed = (rseed * 214013 + 2531011);
  return squeeze(n);
}

// return a random number < n
uint64_t rand_range (uint64_t n) {
  uint64_t x;
  do {
    x=modulo (random (getbits(n)), n);
  } while (x == 0);
  return x;
}

// Sophie Germain prime
uint64_t safe_prime (uint64_t n)
{
  uint64_t p;
  int prime=0;
  
  do {
    p=gen_prime (n/2);
    prime = (fermat ((2*p)+1, 20) && miller ((2*p)+1, 20));
  } while (!prime);
  return (p*2)+1;
}

// Fermat's test for checking primality
int fermat (uint64_t p, int iterations) {
  int i;
  uint64_t a, bitlen=getbits(p);
  
  if (p == 1) {
    return 0;
  }
  for (i=0; i<iterations; i++) {
    a = modulo (random(bitlen), (p-1)) + 1; 
    if (powmod (a, p-1, p) != 1) { 
      return 0;
    }
  }
  return 1;
}

// Miller-Rabin primality test
int miller (uint64_t p, int iteration) {
  uint64_t a, m, s, t, bitlen=getbits(p);
  int i;
  
  if (p < 2) {
    return 0;
  }
  if (p != 2 && (p & 1) == 0) {
    return 0;
  }
  s = p-1;
  while ((s & 1) == 0) {
    s >>= 1;
  }
  for (i=0; i<iteration; i++) {
    a=random(bitlen) % (p-1)+1;
    t=s;
    m=powmod(a, t, p);
    while (t != p-1 && m != 1 && m != p-1) {
      m=mulmod(m, m, p);
      t <<= 1;
    }
    if (m != p-1 && (t & 1) == 0) {
      return 0;
    }
  }
  return 1;
}

// generate a prime number
// this isn't a safe RNG as it uses 
// LCG (Linear Congruent Generator) 
// with time(0) as seed.
uint64_t gen_prime (uint64_t n) {
  uint64_t x;
  int prime=0;
  
  do {
    x=random(n);
    //printf ("\nGen checking %llu", x);
    prime=(fermat (x, 20) && miller (x, 20));
  } while (!prime);
  return x;
}

// greatest common divisor
uint64_t gcd (uint64_t a, uint64_t b)
{
  uint64_t c;
  
  while (a != 0) {
    c = a; 
    a = modulo (b, a);  
    b = c;
  }
  return b;
}

// least common multiple
uint64_t lcm (uint64_t a, uint64_t b) {
  uint64_t q, r, x, y;
  
  x=multiply (a, b);
  y=gcd (a, b);
  divide (&q, &r, x, y);
  return q;
}

// Pollard's Rho algorithm 
uint64_t rho (uint64_t n)
{
  uint64_t c, d, x, xx;
  
  x=rand_range (n);
  c=rand_range (n);
  xx=x;
  
  if (modulo (n, 2) == 0) return 2;
  
  do {
    x=sqrmod (x, n);
    x=addmod (x, c, n);
    
    xx=sqrmod (xx, n);
    xx=addmod (xx, c, n);
    
    xx=sqrmod (xx, n);
    xx=addmod (xx, c, n);
    
    d=subtract (x, xx);
    d=gcd (d, n);
  } while (d == 1);
  
  return d;
}

// return nth root of a 64-bit number
// max number for cube root is 18446724184312856125
// this is based on some code for matasano challenges
uint64_t nroot (uint64_t i_num, uint64_t i_n) {
  uint64_t N, R, x, t0, t1, t2, p;
  
  t0 = i_num;
  x  = i_num;
  N  = subtract (i_n, 1);
  
  while (t0 > 0) {
    p  = x;
    t0 = multiply(N, p);
    t1 = power(p, N);
    divide(&t2, &R, i_num, t1);
    t1 = add(t0, t2);
    divide(&x, &R, t1, i_n);
    t0 = subtract(p, x);
  }
  return x;
}
  
