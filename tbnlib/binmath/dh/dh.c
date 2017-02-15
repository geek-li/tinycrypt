


// Diffie-Hellman-Merkle key exchange
// Odzhan

#include "binmath.h"

#define RP 32   // for creating modulus

int main (void)
{
  uint64_t p, q, g, t, x=0, y=0, A, B, S1, S2;

  puts ("\n  Diffie-Hellman-Merkle Key Exchange example");
  
  printf ("\n  Generating p...");
  // generate safe prime p
  p=safe_prime (RP);
  
  printf ("%llu\n  Generating g...", p);
  
  // create generator.
  q=(p-1)/2;
  do {
    t=rand_range (p);
    g=powmod (t, (p-1)/q, p);
  } while (g <= 1);
  
  printf ("%llu\n", g);
  
  // Alice and Bob agree to use same p and g for key exchange
  // Both generate a random integer to establish session key.
  x=rand_range (p);
  y=rand_range (p);
  
  printf ("\n  Alice has private key %llu", x);
  printf ("\n  Bob   has private key %llu", y);
  
  // Alice computes g ^ x mod p
  A=powmod (g, x, p);
  
  // Bob computes g ^ y mod p
  B=powmod (g, y, p);
  
  // Alice and Bob exchange A and B values
  printf ("\n\n  Alice will send %llu", A);
  printf ("\n  Bob   will send %llu", B);
  
  // Alice computes session key using B ^ x mod p
  S1 = powmod (B, x, p);
  printf ("\n\n  Alice has session key %llu", S1);
  
  // Bob computes session key using A ^ x mod p
  S2 = powmod (A, y, p);
  printf ("\n  Bob   has session key %llu", S2);
  
  // S1 and S2 should be equal
  printf ("\n\n  Key exchange %s\n", 
    S1 == S2 ? "succeeded" : "failed");
  return 0;
}
