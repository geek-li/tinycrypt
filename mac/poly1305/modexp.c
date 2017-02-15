
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

#include "modexp.h"

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

// test bit in x
int bn_bt(bn *x, uint32_t bit) {
  return x->v32[bit>>5] & (1 << (bit & 31));
}

// find max bit in x
int bn_bits(bn *x) {
  int bits;

  for (bits=BIT_LEN-1; bits>=0; bits--) {
    if (bn_bt(x, bits)) break;
  }
  return bits+1;
}

// zero initialize x
void bn_zero(bn *x) {
  __stosb (x->v8, 0, BUF_LEN);
}

// copy s to d
void bn_copy(bn *d, bn *s) {
  uint32_t i;

  bn_zero(d);

  __movsb (d->v8, s->v8, BUF_LEN);
}

// add b to a and store in r
void bn_add(bn *r, bn *a, bn *b) {
  uint32_t i, cy=0;
  bn t;
  bd x;

  bn_zero(&t);

  for (i=0; i<BIT_LEN/32; i++) {
    // x = a + b + carry
    x.v64 = (uint64_t)((uint64_t)a->v32[i] +
            (uint64_t)b->v32[i]) + cy;
    // save x
    t.v32[i] = x.lo;
    // carry?
    cy = (x.hi!=0);
  }
  bn_copy(r, &t);
}

// subtract b from a and store in r
void bn_sub(bn *r, bn *a, bn *b) {
  uint32_t x1, x2, i, cy=0;
  bn t;

  bn_zero(&t);

  for (i=0; i<BIT_LEN/32; i++) {
    x1 = a->v32[i];
    x2 = b->v32[i];
    if (cy) {
      cy = (x1 <= x2);
      x1 = (x1 - x2 - 1);
    } else {
      cy = (x1 < x2);
      x1 = (x1 - x2);
    }
    t.v32[i] = x1;
  }
  bn_copy(r, &t);
}

// compare a and b
//
// return  0 if a == b
// return  1 if a > b
// return -1 if a < b
int bn_cmp(bn *a, bn *b) {
  int i, cmp=0;

  for (i=(BIT_LEN/32)-1; i>=0; --i) {
    // continue while equal
    if (a->v32[i] == b->v32[i]) continue;
    cmp = (a->v32[i] > b->v32[i]) ? 1 : ~0UL;
    break;
  }
  return cmp;
}

// modular addition where:
// 1) a, b and m are non-negative
// 2) a and b are less than m
void bn_addmod(bn *r, bn *a, bn *b, bn *m) {
  bn_add(r, a, b);
  // greater than or equal to modulus?
  if (bn_cmp(r, m)>=0) {
    // subtract from r
    bn_sub(r, r, m);
  }
}

// modular multiplication
void bn_mulmod(bn *x, bn *b, bn *e, bn *m) {
  int i, bits=bn_bits(e);
  bn  t, r;

  bn_zero(&r);
  bn_zero(&t);
  bn_copy(&t, b);

  for (i=0; i<bits; i++) {
    if (bn_bt(e, i)) {
      bn_addmod(&r, &r, &t, m);
    }
    bn_addmod(&t, &t, &t, m);
  }
  bn_copy(x, &r);
}

// modular exponentiation
void bn_expmod(bn *x, bn *b, bn *e, bn *m) {
  int i, bits=bn_bits(e);
  bn  t, r;

  bn_zero(&r);               // initialize result to zero
  bn_zero(&t);               // same with t
  bn_copy(&t, b);            // copy b to t

  r.v32[0] = 1;              // set result to 1

  // for each bit of the exponent
  for (i=0; i<bits; i++) {
    // is i-th bit set?
    if (bn_bt(e, i)) {
      bn_mulmod(&r, &r, &t, m);
    }
    bn_mulmod(&t, &t, &t, m);
  }
  bn_copy(x, &r);
}
