

// DSA signing/verification example using openssl
// Odzhan

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/bn.h>


int main (void)
{
  BIGNUM *p, *q, *g;
  BIGNUM *t, *x, *y, *k, *r, *s, *kinv, *xr, *H;
  BIGNUM *v, *w, *u1, *u2;
  BN_CTX *ctx;
  int i;
  
  ctx=BN_CTX_new();
  
  p=NULL;
  q=NULL;
  g=NULL;
  
  // generate random x
  x=BN_new();
  BN_rand_range (x, q);
  
  // y = g ^ x mod p
  y=BN_new();
  BN_mod_exp(y, g, x, p, ctx);

  printf ("\n\npublic key = %s\nPrivate key = %s", 
    BN_bn2hex(y), BN_bn2hex(x));
  
  // sign message
  t=BN_new();
  k=BN_new();
  kinv=BN_new();
  r=BN_new();
  xr=BN_new();
  s=BN_new();
  H=NULL;
  
  BN_hex2bn (&H, "AB");
  
  while (1) {
    // generate k
    BN_rand_range (k, q);
    
    // inverse k mod q
    BN_mod_inverse (kinv, k, q, ctx);
    // r = (g exp k mod p) mod q
    BN_mod_exp (t, g, k, p, ctx);
    BN_mod (r, t, q, ctx);
    if (BN_is_zero(r)) 
      continue;
    // s = (kinv * (H + x*r)) % q
    BN_mul (xr, x, r, ctx);
    BN_add (s, xr, H);
    BN_mod_mul (s, s, kinv, q, ctx);
    if (!BN_is_zero(s)) break;
  }
  printf ("\n\n(k=%s, r=%s, s=%s)", 
    BN_bn2hex(k), BN_bn2hex(r), BN_bn2hex(s));
  
  BN_free (xr);
  BN_free (t);
  BN_free (k);
  BN_free (kinv);
  
  // now verify
  
  // w = s invmod q
  w=BN_new();
  BN_mod_inverse(w, s, q, ctx);
  // u1 = (H * w) % q
  u1=BN_new();
  BN_mod_mul(u1, H, w, q, ctx);
  // u2 = (r * w) % q
  u2=BN_new();
  BN_mod_mul(u2, r, w, q, ctx);
  // v = ((g^u1 * y^u2) mod p) mod q
  BN_mod_exp (u1, g, u1, p, ctx);
  BN_mod_exp (u2, y, u2, p, ctx);
  v=BN_new();
  BN_mod_mul (v, u1, u2, p, ctx);
  BN_mod (v, v, q, ctx);
  
  printf ("\nv = %s", BN_bn2hex(v));
  printf ("\n  signature is %s", 
    !BN_cmp (r, v) ? "valid" : "invalid");
    
  BN_free (r);
  BN_free (s);
  BN_free (q);
  BN_free (p);
  BN_free (g);
  BN_free (v);
  BN_free (u2);
  BN_free (u1);
  BN_free (w);
  BN_free (H);
  BN_free (x);
  BN_free (y);
  
  BN_CTX_free (ctx);
}
