
// test unit for m32.asm
// requires openssl

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <openssl/bn.h>
#include <openssl/rand.h>

char OAKLEY_PRIME_MODP768[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A63A3620FFFFFFFFFFFFFFFF";
  
char OAKLEY_PRIME_MODP1024[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
	"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
	"FFFFFFFFFFFFFFFF";
    
#define BN_LEN 1024  // should be enough for 8192-bits

// just to test modexp asm function
typedef struct _dh_t {
  uint32_t maxbits;
  uint32_t maxbytes;
  
  // p + g group parameters
  uint8_t p[BN_LEN];
  uint8_t g[BN_LEN];
  
  // private keys
  uint8_t x[BN_LEN];
  uint8_t y[BN_LEN];
  
  // public keys
  uint8_t A[BN_LEN];
  uint8_t B[BN_LEN];
  
  // session keys
  uint8_t s1[BN_LEN];
  uint8_t s2[BN_LEN];
  
} DH_T;

// b=base, e=exponent, m=modulus, r=result
void modexp (uint32_t maxbits, void *b, void *e, void *m, void *r);

void dump_bn (char txt[], void *buf, int len)
{
  int i;
  
  printf ("%s", txt);
  
  for (i=len-1; i>=0; i--) {
    printf ("%02X", ((uint8_t*)buf)[i]);
  }
}

void dh_asm (uint32_t numlen, void *p, void *g, void *x, void *y)
{
  DH_T dh;
  uint32_t len=numlen>>3;
  
  // zero init
  memset (&dh, 0, sizeof (dh));
  
  dh.maxbits=(numlen&-32)+32;
  dh.maxbytes=dh.maxbits>>3;
  
  // group parameters
  memcpy (dh.p, p, len);
  memcpy (dh.g, g, 1);             // this is presumed to be number 2
  
  // then private keys
  memcpy (dh.x, x, len);
  memcpy (dh.y, y, len);
  
  // Alice obtains A = g ^ x mod p
  modexp (dh.maxbytes, dh.g, dh.x, dh.p, dh.A);
  
  // Bob obtains B = g ^ y mod p
  modexp (dh.maxbytes, dh.g, dh.y, dh.p, dh.B);
  
  // *************************************
  // Bob and Alice exchange A and B values
  // *************************************
  
  // Alice computes s1 = B ^ x mod p
  modexp (dh.maxbytes, dh.B, dh.x, dh.p, dh.s1);
  
  // Bob computes   s2 = A ^ y mod p
  modexp (dh.maxbytes, dh.A, dh.y, dh.p, dh.s2);
  
  // s1 + s2 should be equal
  if (memcmp (dh.s1, dh.s2, len)==0) {
    printf ("\n\nx86 asm Key exchange succeeded");
    dump_bn ("\n\nx86 Session key = ", dh.s1, len);
  } else {
    printf ("\nx86 Key exchange failed\n");
  }
}
  
void dh (char modp[])
{
  BIGNUM *p, *g, *x, *y, *A, *B, *s1, *s2;
  BN_CTX *ctx;
  uint32_t maxbits;
  
  // initialize context
  ctx = BN_CTX_new ();
  BN_CTX_init (ctx);
  
  p  = BN_new ();
  g  = BN_new ();
  x  = BN_new ();
  y  = BN_new ();
  A  = BN_new ();
  B  = BN_new ();
  s1 = BN_new ();
  s2 = BN_new ();
  
  BN_hex2bn (&p, modp);
  BN_hex2bn (&g, "2");
  
  // generate 2 random integers in range of p
  BN_rand_range (x, p);
  BN_rand_range (y, p);
  
  puts ("\n\n***********************************");
  printf ("\nP is %i-bits", BN_num_bits (p));
  
  printf ("\np = %s", BN_bn2hex (p));
  printf ("\ng = %s", BN_bn2hex (g));
  printf ("\nx = %s", BN_bn2hex (x));
  printf ("\ny = %s", BN_bn2hex (y));
  
  // Alice does g ^ x mod p
  BN_mod_exp (A, g, x, p, ctx);

  printf ("\nA = %s", BN_bn2hex (A));
  
  // Bob does g ^ y mod p
  BN_mod_exp (B, g, y, p, ctx);

  printf ("\nB = %s", BN_bn2hex (B));
  
  // *************************************
  // Bob and Alice exchange A and B values
  // *************************************
  
  // Alice computes session key
  BN_mod_exp (s1, B, x, p, ctx);

  printf ("\ns1 = %s", BN_bn2hex (s1));
  
  // Bob computes session key
  BN_mod_exp (s2, A, y, p, ctx);
  
  printf ("\ns2 = %s", BN_bn2hex (s2));
  
  // check if both keys match
  if (BN_cmp (s1, s2) == 0) {
    printf ("\n\nKey exchange succeeded");
    printf ("\n\nSession key = %s\n", BN_bn2hex (s1));
  } else {
    printf ("\n\nKey exchange failed\n");
  }
  
  // call the assembler function
  dh_asm (BN_num_bits(p), &p->d[0], &g->d[0], &x->d[0], &y->d[0]);

  BN_free (s2);
  BN_free (s1);
  BN_free (p);
  BN_free (g);
  BN_free (y);
  BN_free (x);
  BN_free (B);
  BN_free (A);
  
  // release context
  BN_CTX_free (ctx);
}
    
uint64_t cntbits (uint64_t *v)
{
  uint64_t i, x=64;
  
  while (--x) {
    if (_bittest ((const LONG*)v, x)) break;
  }
  return x;
}

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

// Modular Addition
uint64_t addmod (uint64_t a, uint64_t b, uint64_t m)
{
  a = a + b;
  if (a >= m) {
    a = a - m;
  }
  return a;
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
uint64_t powmod (uint64_t base, uint64_t exponent, uint64_t modulus)
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

uint64_t xmod (uint64_t b, uint64_t e, uint64_t m, uint64_t x)
{
  uint64_t t=b, r=x;
  int n, i, j;
  
  n=cntbits(&e);

  for (i=0; n>=0; --n, i++)
  {
    if (_bittest((const LONG*)&e, i)) {
      r = (x==1) ? xmod (r, t, m, 0) : addmod (r, t, m);
    }
    t = (x==1) ? xmod (t, t, m, 0) : addmod (t, t, m);
  }
  return r;
}

int main (int argc, char *argv[]) 
{
  uint64_t b, e, m, x=0, y=0;
  
  if (argc==4) {
    b=dec2bin (argv[1]);
    e=dec2bin (argv[2]);
    m=dec2bin (argv[3]);
  
    // sanity check
    if (b>m || e>m) {
      printf ("\nbase and exponent must be less than modulus");
      return 0;
    }
    
    modexp(8, &b, &e, &m, &x);
    y=powmod(b, e, m);
    
    printf ("\nxmod ()  : %lld = %lld ^ %lld %% %lld\n", x, b, e, m);
    printf ("\npowmod() : %lld = %lld ^ %lld %% %lld\n", y, b, e, m);
  } else {
    dh (OAKLEY_PRIME_MODP768);
    dh (OAKLEY_PRIME_MODP1024);
  }
  return 0;
}
