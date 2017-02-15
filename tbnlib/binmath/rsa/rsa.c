


// RSA example using functions in binmath.c

#include "binmath.h"

#define RN 32 // for creating modulus

// sign a message
// normally we'd use a cryptographic hash function
// to compress the message into n-bit value which is
// then encrypted with private key and sent with message
// to recipient who decrypts signature using public key
//
// M is message
// K is private key
// N is modulus
//
// return S as signature
//
uint64_t sign (char M[], uint64_t K, uint64_t N) {
  uint64_t H1=modulo(hash (M), N);   // % N only to avoid overflow
  return powmod (H1, K, N);
}

// return 1 (TRUE) or 0 (FALSE) if S was signed by owner of public key
int verify (char M[], uint64_t S, uint64_t K, uint64_t N) {
  uint64_t H1, H2;
  // calculate hash of message
  H1=modulo(hash (M), N);
  // decrypt signature
  H2=powmod (S, K, N);
  // compare our hash with decrypted signature
  return H1 == H2;
}

int main (int argc, char *argv[])
{
  uint64_t p, q, e, n, phi, d, s;
  char *m;
  
  puts ("\n  RSA example");
  
  if (argc != 2) {
    printf ("\n  usage: rsa <message>\n");
    return 0;
  }

  m=argv[1];
  
  // generate 2 RN-bit primes
  printf ("\n  Generating p...");
  p=safe_prime(RN);
  printf ("%llu", p);
  
  printf ("\n  Generating q...");
  q=safe_prime(RN);
  printf ("%llu\n", q);
  
  // compute modulus, n = pq
  n=multiply (p, q);
  printf ("\n  modulus        : %llu [%0llX]", n, n);
  printf ("\n  message        : \"%s\"", m);
  
  // (p-1) * (q-1)
  // phi=totient (n);
  phi=multiply (p-1, q-1);

  // find e
  e=65537;
  while (gcd (e, phi) != 1) 
    e++;
  
  printf ("\n  encryption key : %llu [%0llX]", e, e);
  
  // d = (e ^ -1) % phi
  d=invmod (e, phi);
  printf ("\n  decryption key : %llu [%0llX]", d, d);
  
  s=sign (m, e, n);
  printf ("\n  signature      : %llu [%0llX] (%s)\n", s, s, 
    verify (m, s, d, n) ? "good" : "bad");
  return 0;
}


