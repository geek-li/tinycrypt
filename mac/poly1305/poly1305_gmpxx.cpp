/*
D. J. Bernstein, 20050113.
Public domain.

Uses GMP's C++ interface to compute Poly1305_r(m,s).
Requires GMP 4 or later for the C++ interface.
Painfully slow.
*/

#include <gmpxx.h>
#include "poly1305_gmpxx.h"

void poly1305_gmpxx(unsigned char *out,
  const unsigned char *r,
  const unsigned char *s,
  const unsigned char *m,unsigned int l)
{
  unsigned int j;
  mpz_class rbar = 0;
  for (j = 0;j < 16;++j)
    rbar += ((mpz_class) r[j]) << (8 * j);
  mpz_class h = 0;
  mpz_class p = (((mpz_class) 1) << 130) - 5;
  while (l > 0) {
    mpz_class c = 0;
    for (j = 0;(j < 16) && (j < l);++j)
      c += ((mpz_class) m[j]) << (8 * j);
    c += ((mpz_class) 1) << (8 * j);
    m += j; l -= j;
    h = ((h + c) * rbar) % p;
  }
  for (j = 0;j < 16;++j)
    h += ((mpz_class) s[j]) << (8 * j);
  for (j = 0;j < 16;++j) {
    mpz_class c = h % 256;
    h >>= 8;
    out[j] = c.get_ui();
  }
}
