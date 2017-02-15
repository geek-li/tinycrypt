


// ElGamal Encryption

#include "binmath.h"

#define RN 16

int main (int argc, char *argv[])
{
  uint64_t h1, h2, t, p, q, g, x, Y, s, k, c1, c2;
  char *m;
  
  puts ("\n  ElGamal Encryption example");
  
  if (argc != 2) {
    printf ("\n  usage: elgamal <message>\n");
    return 0;
  }
  
  m=argv[1];
  
  // generate p
  printf ("\n  Generating p...");
  p=safe_prime (RN); 
  
  // create generator
  printf ("\n  Generating g...\n");
  q=(p-1)/2;
  do {
    t=rand_range (p);
    g=powmod (t, p-1/q, p);
  } while (g <= 1);
  
  // create hash of string mod p
  h1=modulo (hash(m), p);
  x=gen_prime(8);
  
  printf ("\n  Prime          : %llu", p);
  printf ("\n  Generator      : %llu", g);
  printf ("\n  Message        : \"%s\"", m);
  printf ("\n  Hash           : %llu", h1);
  
  // Alice calculates Y = g ^ x
  Y=powmod (g, x, p);
  
  // Alice publishes Y, g and p
  
  // Bob wants to send message to Alice
  // Choose random k
  k=rand_range (p);
  
  // s = Y ^ k
  s=powmod (Y, k, p);
  printf ("\n\n  B: Shared secret  : %llu", s);
  
  // c1 = g ^ k
  c1=powmod (g, k, p);
  
  printf ("\n  B: Plaintext Hash : %llu", h1);
  
  // c2 = h1 * s
  c2=mulmod (h1, s, p);
  
  printf ("\n  B: Encrypted Hash : %llu %llu", c1, c2);
  
  // Bob sends c1 + c2 to Alice
  // Then to decrypt, Alice does
  
  // s = c1 ^ x
  s=powmod (c1, x, p);
  printf ("\n  A: Shared secret  : %llu", s);
  
  // s = s ^ -1
  s=invmod (s, p);

  // h = s * c2
  h2=mulmod (s, c2, p);
  
  printf ("\n  A: Decrypted Hash : %llu", h2);
  printf ("\n\n  Decryption %s\n", h1==h2 ? "succeeded" : "failed");
  return 0;
}

