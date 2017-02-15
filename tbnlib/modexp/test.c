

// Odzhan

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// converts decimal string to 64-bit integer.
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

int bits (uint64_t a) {
  int n=0;
  while (a) {
    a >>= 1;
    n++;
  }
  return n;
}

// Modular Addition
uint64_t addmod (uint64_t a, uint64_t b, uint64_t m)
{
  a = a + b;
  if (a >= m) {
    a = a - m;
  }
  return a;
}

// r
// t
// i
// n
// e
// x
// m

int cntbits (uint64_t *v)
{
  int i, x=64;
  
  while (--x) {
    if (_bittest (v, x)) break;
  }
  return x;
}

uint64_t xmod (uint64_t b, uint64_t e, uint64_t m, uint64_t x)
{
  uint64_t t=b, r=x;
  int n, i, j;
  
  n=cntbits(&e);

  for (i=0; n>=0; --n, i++)
  {
    if (_bittest(&e, i)) {
      r = (x==1) ? xmod (r, t, m, 0) : addmod (r, t, m);
    }
    t = (x==1) ?   xmod (t, t, m, 0) : addmod (t, t, m);
  }
  return r;
}

// Modular multiplication
uint64_t mulmod (uint64_t num1, uint64_t num2, uint64_t modulus)
{
  uint64_t r=0, b=num1, e=num2, m=modulus;

  while (e > 0) {
    if (e & 1) {
      r = addmod (r, b, m);
    }
    b = addmod (b, b, m);
    e >>= 1;
  }
  return r;
}

// Modular exponentiation
uint64_t expmod2 (uint64_t base, uint64_t exponent, uint64_t modulus)
{
  uint64_t r=1, b=base, e=exponent, m=modulus;
  
  while (e > 0) {
    if (e & 1) {
      r = mulmod (r, b, m);
    }
    b = mulmod (b, b, m);
    e >>= 1;
  }
  return r;
}

// Modular multiplication
/**
uint64_t mulmodp (uint64_t b, uint64_t e, uint64_t m)
{
  uint64_t r=0, t=b;
  int64_t n, i;
  
  n=bits (e);
  
  for (i=0; i<=n; i++) {
    if (e & (1 << i)) {
      r = addmod (r, t, m);
    }
    t = addmod (t, t, m);
  }
  return r;
}*/

// Modular exponentiation
uint64_t expmod (uint64_t b, uint64_t e, uint64_t m)
{
  uint64_t r=1, t=b;
  
  while (e) {
    if (e & 1) {
      r = (r + t) % m;
    }
    t = (t + t) % m;
    e >>= 1;
  }
  return r;
}

int main (int argc, char *argv[])
{
  uint64_t b, e, m, x, y, z, q;
  
  if (argc != 4) {
    printf ("\nusage: modexp <base> <exponent> <modulus\n");
    return 0;
  }
  
  // convert to binary
  b=dec2bin (argv[1]);
  e=dec2bin (argv[2]);
  m=dec2bin (argv[3]);
  
  // sanity check
  if (b>m || e>m) {
    printf ("\nbase and exponent must be less than modulus");
    return 0;
  }
  // perform modular exponentiation
  x=xmod (b, e, m, 1);
  y=expmod (b, e, m);
  z=expmod2 (b, e, m);
  
  printf ("\nxmod ()  : %lld = %lld ^ %lld %% %lld\n", x, b, e, m);
  printf ("\nexpmod() : %lld = %lld ^ %lld %% %lld\n", y, b, e, m);
  printf ("\nexpmod2() : %lld = %lld ^ %lld %% %lld\n", z, b, e, m);
  
  return 0;
}
