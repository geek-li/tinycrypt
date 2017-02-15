
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

#include "tbnlib.h"

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#pragma intrinsic(memset)

#if defined(_WIN32) || defined(_WIN64)
#define WINX
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
  uint32_t i;
  
  x->neg=0;
  
// 64-bit code seems to use memset regardless of compiler flags
#if defined(WINX)
  __stosd(x->v32, 0, BUF_LEN/32);
#else
  for (i=0; i<BUF_LEN/32; i++) {
    x->v32[i] = 0;
  }
#endif
}

// copy s to d
void bn_copy(bn *d, bn *s) {
  uint32_t i;

  bn_zero(d);

  d->neg = s->neg;
  
#if defined(WINX)
  __movsd(d->v32, s->v32, BUF_LEN/32);
#else
  for (i=0; i<BUF_LEN/32; i++) {
    d->v32[i] = s->v32[i];
  }
#endif
}

// unsigned addition
void bn_uadd (bn *out, bn *a, bn *b)
{
  bn t;
  
  bn_zero(&t);
  
  for (i=0; i<BIT_LEN/32; i++) {
    sum = a->v32[i] + b->v32[i] + cy;

    cy = (sum < a->v32[i]) | ((sum == a->v32[i]) && cy);
    t.v32[i] = sum;
  }
  bn_copy(out, &t);  
}

// signed addition
void bn_add(bn *out, bn *a, bn *b) {
  bn *tmp;

  if (a->neg ^ b->neg)
  {
    // if only one is negative
    if (a->neg) {
      tmp = a;
      a = b;
      b = tmp;
    }
    // now we are a - b
    if (bn_ucmp(a, b) < 0) 
    {
      if (!bn_usub(out, b, a)) {
        return 0;
      }
      r->neg=1;
    } else {
      if (!bn_usub(out, a, b)) {
        return 0;
      }
      r->neg=0;
    }
    return 1;
  }
  ret = bn_uadd(out, a, b);
  out->neg = a->neg;
  
  return ret;
}

// subtract b from a and store in r
void bn_usub(bn *out, bn *a, bn *b) {
  uint32_t diff, i, cy=0;
  bn t;
  bn *l=a, *r=b, *v;
  
  bn_zero(&t);

  for (i=0; i<BIT_LEN/32; i++) {
    diff = l->v32[i] - r->v32[i] - cy;

	  cy = (diff > l->v32[i]) | ((diff == l->v32[i]) && cy);
		t.v32[i] = diff;    
  }
  bn_copy(out, &t);
}

// signed subtraction
void bn_sub() {
  int add=0, neg=0;
  bn  *tmp;
  
    /*-
     *  a -  b      a-b
     *  a - -b      a+b
     * -a -  b      -(a+b)
     * -a - -b      b-a
     */  
  if (a->neg) {
    if (b->neg) {
      tmp = a;
      a = b;
      b = tmp;
    } else {
      add = 1;
      neg = 1;
    }
  } else {
    if (b->neg) {
      add = 1;
      neg = 0;
    }
  }
  
  if (add) {
    if (!bn_uadd(out, a, b)) {
      return 0;
    }
    out->neg = neg;
    return 1;
  }
  return 1;
}

// unsigned compare a and b
//
// return  0 if a == b
// return  1 if a > b
// return -1 if a < b
int bn_ucmp(bn *a, bn *b) {
  int i, cmp=0;

  for (i=(BIT_LEN/32)-1; i>=0; --i) {
    // continue while equal
    if (a->v32[i] == b->v32[i]) continue;
    cmp = (a->v32[i] > b->v32[i]) ? 1 : ~0UL;
    break;
  }
  return cmp;
}

// signed compare
int bn_cmp(bn *a, bn *b) {
  int i, gt, lt, cmp; 
  
  if (a->neg != b->neg) {
    if (a->neg) {
      return -1;
    } else {
      return 1;
    }
  }
  if (a->neg == 0) {
    gt = 1; lt = -1;
  } else {
    gt = -1; lt = 1;    
  }
  for (i=(BIT_LEN/32)-1; i>=0; --i) {
    // continue while equal
    if (a->v32[i] == b->v32[i]) continue;
    cmp = (a->v32[i] > b->v32[i]) ? gt : lt;
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

  bn_zero(&r);            // r=0
  bn_zero(&t);            // t=0
  bn_copy(&t, b);         // t=b

  for (i=0; i<bits; i++) {
    // test exponent bit
    if (bn_bt(e, i)) {
      // add t to result within m
      bn_addmod(&r, &r, &t, m);
    }
    // add t to t within m
    bn_addmod(&t, &t, &t, m);
  }
  bn_copy(x, &r);
}

// modular exponentiation
void bn_expmod(bn *x, bn *b, bn *e, bn *m) {
  int i, bits=bn_bits(e);
  bn  t, r;

  bn_zero(&r);    // r=0
  bn_zero(&t);    // r=0
  bn_copy(&t, b); // t=b

  // set result to one
  r.v32[0] = 1;

  for (i=0; i<bits; i++) {
    // test exponent bit
    if (bn_bt(e, i)) {
      // multiply result by base within m
      bn_mulmod(&r, &r, &t, m);
    }
    // multiply base within m
    bn_mulmod(&t, &t, &t, m);
  }
  // return result
  bn_copy(x, &r);
}

#ifndef COMPACT
// Multiply x by y and store in r
// x and y should not exceed BIT_LEN/2 bits
void bn_mul (bn *r, bn *a, bn *b) {
  int      i, j;
  uint16_t cy;
  uint16_t *p;
  bn       t;
  bw       x;

  // zero temp space
  bn_zero(&t);

  for (i=0; i<BIT_LEN/16; i++)
  {
    for (j=0; j<BIT_LEN/16; j++)
    {
      // store multiplication as 32-bit value
      x.v32 = (uint32_t)((uint32_t)a->v16[i] * (uint32_t)b->v16[j]);
      p = &t.v16[i+j];

      // save lo 16-bits
      // and test for carry
      x.lo += *p;
      cy = (x.lo < *p);
      *p++ = x.lo;

      // save hi 16-bits plus carry
      // continue adding for each carry
      do {
        x.hi += *p + cy;
        cy = (x.hi < *p);
        *p++ = x.hi;
        x.hi = 0;
      } while (cy);
    }
  }
  bn_copy(r, &t);
}

// modulo dd by dr
// store remainder in r_out
void bn_mod(bn *r_out, bn *dd, bn *dr) {
  int i, bits;
  bn  r;

  bn_zero(&r);

  bits=bn_bits(dd);

  for (i=bits-1; i>=0; --i) {
    // double r
    bn_add(&r, &r, &r);
    // test bit
    if (bn_bt(dd, i)) {
      // update remainder
      r.v8[0] |= 1;
    }
    // if r exceeds divisor
    if (bn_cmp(&r, dr) >= 0) {
      // subtract
      bn_sub(&r, &r, dr);
    }
  }
  bn_copy(r_out, &r);
}

// return number of bytes for bn
int bn_bytes(bn *x) {
  int bits;

  bits=bn_bits(x);
  
  if (bits) {
    if ((bits & 7)!=0) {
      // align by 8
      bits  = (bits & -8) + 8;
    }
    // return bytes
    bits >>= 3;
  }
  return bits;
}

void bn_not(bn *x) {
  int i;
  for (i=0; i<BIT_LEN/32; i++) {
    x->v32[i] = ~x->v32[i];
  }
}

// divide dd by dr
// store quotient in q_out
// store remainder in r_out
void bn_div(bn *q_out, bn *r_out, bn *dd, bn *dr) {
  int i, bits;
  bn  q, r;
  bn *x, *y, *v;
  
  x=dd;
  y=dr;
  if (bn_cmp(dr, dd) >= 0) {
    //v=x;
    //x=y;
    //y=v;
  }
  // r=0, q=0
  bn_zero(&r);
  bn_zero(&q);

  // get bits of dd
  bits=bn_bits(dd);

  for (i=bits-1; i>=0; i--) {
    // double q and r
    bn_add(&q, &q, &q);
    bn_add(&r, &r, &r);

    // test bit in dd
    if (bn_bt(dd, i)) {
      // update remainder
      r.v8[0] |= 1;
    }
    // if r exceeds divisor
    if (bn_cmp(&r, dr) >= 0) {
      // subtract
      bn_sub(&r, &r, dr);
      // update quotient
      q.v8[0] |= 1;
    }
  }
  bn_copy(q_out, &q);
  bn_copy(r_out, &r);
}


// not working..
void bn_invmod (bn *r, bn *a, bn *m)
{
  bn  b, c, i, j, t, x, y, z;
  int bits;

  bn_copy(&b, m);
  bn_copy(&c, a);

  bn_zero(&i);
  bn_zero(&j);
  bn_zero(&z);

  j.v32[0]=1;

  //uint64_t j = 1, i = 0, b = m, c = a, x, y;

  while (!bn_is_zero(&c))
  {
    // x = b / c;
    // y = b - x * c;
    bn_div(&x, &y, &b, &c);
    bn_copy(&b, &c);
    bn_copy(&c, &y);
    bn_copy(&y, &j);
    // j = i - j * x;
    bn_mul(&t, &j, &x);
    bn_sub(&j, &i, &t);
    bn_copy(&i, &y);
  }
  if (bn_cmp(a, b) < 0) {
    // i += m;
    bn_add (&i, &i, m); 
  }
  bn_copy(r, &i);
}

// add word y to bignum a
void bn_add_word(bn *a, bn_word y) {
  bn_word x=y, cy=0;
  bn_word *p=(bn_word*)a->v8;

  do {
    x  = (*p + x + cy);
    cy = (x < *p);
    *p++ = x;
    x = 0;
  } while (cy);
}

// sub word y from bignum a
void bn_sub_word(bn *a, bn_word y) {
  bn_word x=y, cy=0;
  bn_word *p=(bn_word*)a->v8;

  do {
    x  = (*p - x - cy);
    cy = (x < *p);
    *p++ = x;
    x = 0;
  } while (cy);
}

bn_word bn_bits_word (bn_word w)
{
  bn_word bits=0;

  while (w) {
    w >>= 1;
    bits++;
  }
  return bits;
}

uint32_t bn_div_words(bn*, uint32_t, uint32_t);

// div a by w
// return remainder
bn_word bn_div_word(bn *in, bn_word w) {
  int l=bn_bits(in)>>5;

  return bn_div_words(in, l, w);
}

// mul bignum a by word w
void bn_mul_word(bn *in, bn_word w) {
  bn t;

  bn_zero(&t);
  t.v32[0]=w;
  bn_mul(in, in, &t);
}

#define BIN2HEX(x) (x<10) ? (x + '0') : (x + '7')

// return bn as string
char *bn2str(bn *in, int base) {
  static char r[BIT_LEN/8], s[BIT_LEN/8];
  bn_word x, idx=0;
  uint32_t c, n;
  bn t;
  char *p=r;
  int v, z=0, i, j, len=bn_bits(in);

  memset(r, 0, sizeof(r));
  
  if (in->neg==1) *p++ = '-';
  if (len==0) *p++ = '0';

  len >>= 5;
  
  bn_copy(&t, in);

  if (base==16)
  {
    //printf ("\n%i", len);
    for (i = len; i >= 0; i--) {
      for (j = 32 - 8; j >= 0; j -= 8) {
        v = ((int)(in->v32[i] >> (long)j)) & 0xff;
        if (z || (v != 0)) {
          *p++ = BIN2HEX((v >> 4));
          *p++ = BIN2HEX((v & 15));
          z = 1;
        }
      }
    }
  } else
  if (base==10)
  {
    while (t.v32[0] != 0) {
      c = bn_div_word(&t, 10);
      s[idx++] = c + '0';
    }
  }
  if (base==10)
  {
    p=r;
    while (idx--) {
      *p++ = s[idx];
    }
  }
  return r;
}

// convert str to bn
// supports hexadecimal or decimal
void str2bn(bn *out, int base, char str[]) {
  bn_word x, idx;
  uint8_t b, c;
  int     len;

  bn_zero(out);

  // get length
  for (idx=0, len=0; str[len]!=0; len++);

  // ensure we don't overflow buffer

  while (--len >= 0) {
    // decimal?
    if (base==10) {
      c = *str++;
      bn_mul_word(out, 10);
      x = c - '0';
      bn_add_word(out, x);
    } else
    // hexadecimal?
    if (base==16) {
      c = str[len];
      if (c >= 'a' && c <= 'f')
        c = c - ('a' - 10);
      else if (c >= 'A' && c <= 'F')
        c = c - ('A' - 10);
      else if (c >= '0' && c <= '9')
        c = c - '0';

      if (idx & 1) c <<= 4;

      out->v8[idx>>1] |= c;
      idx++;
    }
  }
}

void bn_swap (bn *x) {
  int i, len=bn_bytes(x);
  uint8_t t;
  
  for (i=0; i<len/2; i++) {
    t = x->v8[i];
    x->v8[i] = x->v8[len-i-1];
    x->v8[len-i-1] = t;
  }
}

#endif