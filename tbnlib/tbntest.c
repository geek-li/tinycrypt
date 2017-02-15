

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tbnlib.h"

#include <openssl/bn.h>

int main(void)
{
  BIGNUM *m, *w, *x, *y, *z;
  BN_CTX *ctx;
  bn      a, b, c, n, r, q;
  
  m   = BN_new();
  w   = BN_new();
  x   = BN_new();
  y   = BN_new();
  z   = BN_new();
  ctx = BN_CTX_new();
  
  bn_zero(&a);
  bn_zero(&b);
  bn_zero(&n);
  
  // =======================  
  printf ("\n a = %s : %i-bits %i bytes", 
    bn2str(&a, 16),
    bn_bits(&a),
    bn_bytes(&a));
    
  printf ("\n x = %s : %i-bits %i bytes", 
    BN_bn2hex(x),
    BN_num_bits(x),
    BN_num_bytes(x));
    
  // random x
  BN_rand (x, 32, -1, 0);  
  BN_bn2bin(x, a.v8);
  
  // random y
  BN_rand (y, 32, -1, 0);
  BN_bn2bin(y, b.v8);
  
  // random m
  BN_rand (m, 64, -1, 0);
  BN_bn2bin(m, n.v8);
  
  bn_swap(&a);
  bn_swap(&b);
  bn_swap(&n);
  
  // =======================  
  printf ("\n a = %s : %i-bits %i bytes", 
    bn2str(&a, 16),
    bn_bits(&a),
    bn_bytes(&a));
    
  printf ("\n x = %s : %i-bits %i bytes", 
    BN_bn2hex(x),
    BN_num_bits(x),
    BN_num_bytes(x));
  // =======================  
  printf ("\n b = %s : %i-bits %i bytes", 
    bn2str(&b, 16),
    bn_bits(&b),
    bn_bytes(&b));
    
  printf ("\n y = %s : %i-bits %i bytes", 
    BN_bn2hex(y),
    BN_num_bits(y),
    BN_num_bytes(y));
  // =======================  
  printf ("\n n = %s : %i-bits %i bytes", 
    bn2str(&n, 16),
    bn_bits(&n),
    bn_bytes(&n));
    
  printf ("\n m = %s : %i-bits %i bytes", 
    BN_bn2hex(m),
    BN_num_bits(m),
    BN_num_bytes(m)); 
  
  printf ("\n\n");
  
  // =======================
  bn_add(&c, &a, &b);
  printf ("\n a + b = %s", bn2str(&c, 16));
  
  BN_add(z, x, y);
  printf ("\n x + y = %s", BN_bn2hex(z));
  
  // =======================
  bn_sub(&c, &a, &b);
  printf ("\n a - b = %s", bn2str(&c, 16));

  BN_sub(z, x, y);
  printf ("\n x - y = %s", BN_bn2hex(z));
  
  // =======================  
  bn_sub(&c, &b, &a);
  printf ("\n b - a = %s", bn2str(&c, 16));

  BN_sub(z, y, x);
  printf ("\n y - x = %s", BN_bn2hex(z));

  // =======================    
  bn_mul(&c, &a, &b);
  printf ("\n a * b = %s", bn2str(&c, 16));

  BN_mul(z, x, y, ctx);
  printf ("\n y - x = %s", BN_bn2hex(z));
  
  // =======================   
  bn_mod(&c, &a, &b);
  printf ("\n a %% b = %s", bn2str(&c, 16));
  
  BN_mod(z, x, y, ctx);
  printf ("\n x %% y = %s", BN_bn2hex(z));  
  
  // ======================= 
  bn_div(&q, &r, &a, &b);
  printf ("\n a / b = %s", bn2str(&q, 16));
  printf ("\n a %% b = %s", bn2str(&r, 16));
  
  BN_div(z, w, x, y, ctx);
  printf ("\n x / y = %s", BN_bn2hex(z));   
  printf ("\n x %% y = %s", BN_bn2hex(w));   
  
  // ======================= 
  bn_expmod(&r, &a, &b, &n);
  printf ("\n a ^ b %% n = %s", bn2str(&r, 16));
  
  BN_mod_exp(z, x, y, m, ctx);
  printf ("\n x ^ y %% m = %s", BN_bn2hex(z));
  
  // ======================= 
  bn_invmod (&c, &a, &n);
  printf ("\n a ^ -1 %% n = %s", bn2str(&c, 16));
  
  BN_mod_inverse(z, x, m, ctx);
  printf ("\n x ^ -1 %% m = %s", BN_bn2hex(z));
  
  BN_free(m);
  BN_free(w);
  BN_free(x);
  BN_free(y);
  BN_free(z);
  
  BN_CTX_free(ctx);
  
  return 0;
}
