

// Diffie-Hellman-Merkle key exchange

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#pragma comment (lib, "user32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "gdi32.lib")

#include <openssl/bn.h>
#include <openssl/rand.h>

const char OAKLEY_PRIME_MODP768[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A63A3620FFFFFFFFFFFFFFFF";

const char OAKLEY_PRIME_MODP1024[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
	"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
	"FFFFFFFFFFFFFFFF";
    
void do_dh (const char *p_value)
{
  BIGNUM *p, *g, *x, *y, *A, *B, *s1, *s2;
  BN_CTX *ctx;

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
  
  BN_hex2bn (&p, p_value);
  BN_hex2bn (&g, "2");
  
  // Alice generates random x
  BN_rand (x, 128, -1, 0);
  
  // Bob generates random y
  BN_rand (y, 128, -1, 0);
  
  // Alice does g ^ x mod p
  BN_mod_exp (A, g, x, p, ctx);
  
  // Bob does g ^ y mod p
  BN_mod_exp (B, g, y, p, ctx);

  // *************************************
  // Bob and Alice exchange A and B values
  // *************************************
  
  // Alice computes session key
  BN_mod_exp (s1, B, x, p, ctx);
  
  // Bob computes session key
  BN_mod_exp (s2, A, y, p, ctx);
  
  // check if both keys match
  if (BN_cmp (s1, s2) == 0) {
    printf ("\n\nKey exchange succeeded");
    printf ("\n\nSession key = %s\n", BN_bn2hex (s1));
  } else {
    printf ("\n\nKey exchange failed\n");
  }
  
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
    
int main (void) 
{
  do_dh (OAKLEY_PRIME_MODP768);
  do_dh (OAKLEY_PRIME_MODP1024);
  
  return 0;
}
