

// SPEKE example using OpenSSL BN Library
// Odzhan

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#if defined (_WIN32) || defined (_WIN64)
 #pragma comment (lib, "user32.lib")
 #pragma comment (lib, "advapi32.lib")
 #pragma comment (lib, "gdi32.lib")
#endif

#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

const char OAKLEY_PRIME_MODP1024[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
	"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
	"FFFFFFFFFFFFFFFF";
    
// derive SHA-256 hash of password    
void hash (uint8_t out[], char *pwd)
{
  SHA256_CTX ctx;
  
  SHA256_Init (&ctx);
  SHA256_Update (&ctx, pwd, strlen (pwd));
  SHA256_Final (out, &ctx);
}

void speke (char *pwd)
{
  BIGNUM *p, *g, *x, *y, *A, *B, *s1, *s2;
  BN_CTX *ctx;
  uint8_t h[SHA256_DIGEST_LENGTH];

  // initialize context
  ctx = BN_CTX_new ();
  BN_CTX_init (ctx);
  
  p=BN_new();
  BN_hex2bn (&p, OAKLEY_PRIME_MODP1024);
  
  // set generator using SHA-256 (pwd) * SHA-256 (pwd) mod p
  hash (h, pwd);
  g=BN_new();
  BN_bin2bn (h, SHA256_DIGEST_LENGTH, g);
  
  BN_mod_sqr (g, g, p, ctx);
  
  // generate 2 128-bit random values, x and y
  x=BN_new();
  BN_rand (x, 128, -1, 0);
  
  y=BN_new();
  BN_rand (y, 128, -1, 0);
  
  // Alice does g ^ x mod p
  A=BN_new();
  BN_mod_exp (A, g, x, p, ctx);
  
  // Bob does g ^ y mod p
  B=BN_new();
  BN_mod_exp (B, g, y, p, ctx);

  // *************************************
  // Bob and Alice exchange A and B values
  // *************************************
  
  // Alice computes session key
  s1=BN_new();
  BN_mod_exp (s1, B, x, p, ctx);
  
  // Bob computes session key
  s2=BN_new();
  BN_mod_exp (s2, A, y, p, ctx);
  
  // check if both keys match
  if (BN_cmp (s1, s2) == 0) {
    printf ("\n  Key exchange succeeded");
    printf ("\n  Key = %s\n", BN_bn2hex (s1));
  } else {
    printf ("\n  Key exchange failed\n");
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
    
int main (int argc, char *argv[]) 
{
  char *pwd="password";
  
  if (argc==2) pwd=argv[1];
  
  puts ("\n  SPEKE example using OpenSSL BN Library");
  
  speke (pwd);
  return 0;
}
