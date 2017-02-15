
// for testing m32.asm
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <openssl/bn.h>
#include <openssl/rand.h>

#define bzero(b, len) (memset((b), '\0', (len)), (void) 0)  
#define bcopy(b1, b2, len) (memmove((b1), (b2), (len)), (void) 0)

extern void modexp(uint32_t numlen, void *base, 
    void *exponent, void *modulus, void *result);

extern void modadd (uint32_t numlen, void *num1, void *num2, void *modulus, void *result);
extern void modmul (uint32_t numlen, void *num1, void *num2, void *modulus, void *result);

// calculate random numbers from 32-8192 bits and compare results of both functions    
// reporting errors if found
void long_test (void)
{
  BIGNUM *b, *e, *m, *r;
  BN_CTX *ctx;
  uint8_t base[1024], exponent[1024], modulus[1024], result[1024];
  uint32_t maxbits;
  int i, j;
  
  // initialize context
  ctx = BN_CTX_new ();
  BN_CTX_init (ctx);
  
  b  = BN_new ();
  e  = BN_new ();
  m  = BN_new ();
  r  = BN_new ();
  
  for (i=128; i<8192; i+=128)
  {
    if ((i % 1024)==0) {
      printf ("\nFinished testing %i-bit", i);
    }
    // generate random numbers for b, e and m ensuring b<m, e<m
    BN_rand (m, i, -1, 0);
    BN_rand_range (b, m);
    BN_rand_range (e, m);
    
    // first using openssl function
    BN_mod_exp (r, b, e, m, ctx);
    
    maxbits = max ((BN_num_bits(b), BN_num_bits(e)), BN_num_bits(m));
    
    bzero (base,     sizeof (base));
    bzero (exponent, sizeof (exponent));
    bzero (modulus,  sizeof (modulus));
    bzero (result,   sizeof (result));
    
    bcopy (base,     &b->d[0], BN_num_bytes (b));
    bcopy (exponent, &e->d[0], BN_num_bytes (e));
    bcopy (modulus,  &m->d[0], BN_num_bytes (m));
    
    // align by 32
    maxbits = (maxbits & -32) + 32;
    maxbits /= 8;
    modexp (maxbits, base, exponent, modulus, result);
    
    if (memcmp (r->d, result, BN_num_bytes (r)))
    {
      printf ("\nTest failed for %i-bit numbers\n", i);

      printf ("\nb=%s e=%s m=%s", BN_bn2hex(b), BN_bn2hex(e), BN_bn2hex(m));
      printf ("\nBN_mod_exp result   = ");
      
      for (j=BN_num_bytes(r)-1; j>=0; j--) {
        printf ("%02X", ((uint8_t*)r->d)[j]);
      }
  
      printf ("\nmodexp32 result     = ");
      
      for (j=BN_num_bytes(r)-1; j>=0; j--) {
        printf ("%02X", result[j]);
      }
  
    }
  }
  BN_free (r);
  BN_free (m);
  BN_free (e);
  BN_free (b);

  // release context
  BN_CTX_free (ctx);
} 
        
int main(int argc, char *argv[])
{
  BIGNUM *b, *e, *m, *c, *x;
  BN_CTX *ctx;
  uint8_t base[256], exponent[256], modulus[256], result[256];
  uint32_t maxbits;
  int i;
  
  if (argc==2 && (argv[1][0]=='b' && argv[1][1]=='m')) {
    long_test();
    return 0;
  }
  
  if (argc!=4) { 
    printf ("\nUsage: mtest <base> <exponent> <modulus>\n"); 
    return 0; 
  }
  
  // initialize context
  ctx = BN_CTX_new ();
  BN_CTX_init (ctx);
  
  b  = BN_new ();
  e  = BN_new ();
  m  = BN_new ();
  c  = BN_new ();
  
  BN_dec2bn (&b, argv[1]);
  BN_dec2bn (&e, argv[2]);
  BN_dec2bn (&m, argv[3]);
  
  BN_mod_exp (c, b, e, m, ctx);
  
  printf ("\nBN_mod_exp result = %s", BN_bn2hex(c));
  
  maxbits = max ((BN_num_bits(b), BN_num_bits(e)), BN_num_bits(m));
  
  bzero (base,     sizeof (base));
  bzero (exponent, sizeof (exponent));
  bzero (modulus,  sizeof (modulus));
  bzero (result,   sizeof (result));
  
  bcopy (base,     &b->d[0], BN_num_bytes (b));
  bcopy (exponent, &e->d[0], BN_num_bytes (e));
  bcopy (modulus,  &m->d[0], BN_num_bytes (m));
  
  // align by 32
  //maxbits = BN_num_bytes (m);
  maxbits = (maxbits & -32) + 32;
  maxbits /= 8;
  modexp (maxbits, base, exponent, modulus, result);
  //putchar('\n');
  
  printf ("\nmodexp result   = ");
  
  for (i=BN_num_bytes(c)-1; i>=0; i--) {
    printf ("%02X", result[i]);
  }
  
  
  //x=BN_bin2bn (result, BN_num_bytes(c), NULL);
  
  /*printf ("\nmodexp32 result   = %s - %s", 
    BN_bn2dec (x), BN_bn2hex(x));
  */
  BN_free (c);
  BN_free (m);
  BN_free (e);
  BN_free (b);
  //BN_free (x);
  
  // release context
  BN_CTX_free (ctx);
  return 0;
}
