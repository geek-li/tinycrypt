


// Schmidt-Samoa cryptosystem

#include "binmath.h"

#define RP 31

int main (int argc, char *argv[])
{
  uint64_t c, d, m, p, q, t, N;
  
  puts ("\n  Schmidt-Samoa cryptosystem example");
  
  if (argc!=2) {
    printf ("\n  usage: ss <message>\n");
    return 0;
  }
  m=hash(argv[1]);
  
  p=safe_prime(RP);
  q=safe_prime(RP);
  
  // N=(p^2)*q
  N=multiply(multiply(p, p), q);
  
  //d = N-1 mod lcm(p-1, q-1)
  d=invmod(N, lcm(p-1, q-1));
  
  // N is the public key and d is the private key
  // before encryption, ensure hash of message
  // is within range of N and p*q
  m=modulo(m, N);
  m=modulo(m, multiply(p, q));
  
  // encrypt m
  c=powmod(m, N, N);
  
  // decrypt c
  t=powmod(c, d, multiply(p, q));
  
  printf ("\n  Prime p       = %llu", p);
  printf ("\n  Prime q       = %llu", q);
  printf ("\n  Public key N  = %llu", N);
  printf ("\n  Private key d = %llu", d);
  printf ("\n  Message m     = %llu", m);
  printf ("\n  Encrypted c   = %llu", c);
  printf ("\n  Decrypted c   = %llu", t);
  
  printf ("\n\n  Encryption/Decryption %s\n",
    t==m ? "succeeded" : "failed");
  return 0;
}
