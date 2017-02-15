

// SPEKE example using OpenSSL DH Library
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
#include <openssl/dh.h>
#include <openssl/sha.h>

const char OAKLEY_PRIME_MODP1024[]=
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
	"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
	"FFFFFFFFFFFFFFFF";
  
DH     *client=NULL, *server=NULL;
BN_CTX *ctx;

// derive SHA-256 hash of password    
void hash (uint8_t out[], char *pwd)
{
  SHA256_CTX ctx;
  
  SHA256_Init (&ctx);
  SHA256_Update (&ctx, pwd, strlen (pwd));
  SHA256_Final (out, &ctx);
}

int init_dh (char *pwd, DH **dh)
{
  uint8_t h[SHA256_DIGEST_LENGTH];
  
  // create parameters
  dh[0]=DH_new();

  // set p prime
  BN_hex2bn (&dh[0]->p, OAKLEY_PRIME_MODP1024);
  //BN_hex2bn (&dh[0]->g, "2");
  
  // set generator using SHA-256 (pwd) * SHA-256 (pwd) mod p
  hash (h, pwd);
  dh[0]->g=BN_new();
  BN_bin2bn (h, SHA256_DIGEST_LENGTH, dh[0]->g);
  
  BN_mod_sqr (dh[0]->g, dh[0]->g, dh[0]->p, ctx);
  
  return DH_generate_key (dh[0]);
}

void dump (uint8_t x[])
{
  size_t i;
  for (i=0; i<128; i++) {
    printf ("%02X", x[i]);
  }
}

int main (int argc, char *argv[])
{
  char    *pwd="password";
  uint8_t s1[512], s2[512];
  BIGNUM  *key1, *key2;
  
  puts ("\n  SPEKE example using OpenSSL DH Library");
  
  if (argc==2) {
    pwd=argv[1];
  }
  
  ctx=BN_CTX_new ();
  
  // initialize DH parameters
  if (init_dh (pwd, &client)==1) 
  {
    if (init_dh (pwd, &server)==1) 
    {
      // server does
      printf ("\n  computing server key");
      DH_compute_key (s1, client->pub_key, server);
      printf ("\n  key 1 : "); dump (s1);
      
      // client does
      printf ("\n\n  computing client key");
      DH_compute_key (s2, server->pub_key, client);
      printf ("\n  key 2 : "); dump (s2);
      
      // keys match? we only check first 32 bytes
      printf ("\n\n  Key exchange %s\n", 
        memcmp (s1, s2, 32)==0 ? "succeeded" : "failed");
    } else {
      printf ("\n  server initialization failed");
    }
  } else {
    printf ("\n  client initialization failed");
  }
  
  if (client!=NULL) DH_free(client);
  if (server!=NULL) DH_free(server);
  
  BN_CTX_free (ctx);
  
  return 0;
}
