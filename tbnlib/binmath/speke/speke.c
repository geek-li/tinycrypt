

// SPEKE (Simple Password Exponential Key Exchange) example using functions in binmath.c
// Odzhan

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "binmath.h"

#define RP 32   // for creating modulus      
      
/**
   For SPEKE:
         S := B^a % N                                  S := A^b % N */
         
void speke (uint64_t x, uint64_t A, uint64_t y, uint64_t B, uint64_t N)
{
  uint64_t S1, S2;
  
  printf ("\n  ---------------------------");
  
  // Alice computes session key using B ^ x % N
  S1 = powmod (B, x, N);
  
  // Bob computes session key using A ^ x % N
  S2 = powmod (A, y, N);
  
  printf ("\n\n  Alice S=%llu", S1);
  printf ("\n  Bob   S=%llu",   S2);  
  
  // S1 and S2 should be equal
  printf ("\n\n  SPEKE Key exchange %s\n", 
    S1 == S2 ? "succeeded" : "failed");
}

/**

  N     field modulus, a suitable 1024-bit prime of the form kq + 1
  q     prime order of the subgroup to be used, larger than 2^159
  k     the "co-factor", k=N-1/q.  It is recommended that all
        divisors of k/2 other than 1 be larger than q.
  hash  SHA-1, or an alternate suitable hash function
  kdf   KDF1-SHA1, as defined in [IEEE 1363], or an alternate
         suitable key derivation function
             
  p := the password or other potentially small authenticator string
  x := any suitable derivation of <p> that preserves its randomness
  g := REDP(x), "REDP" is a random element derivation function
  v := g^x % N        (only used in B-SPEKE and W-SPEKE) */
  
/**
   For BSPEKE:
         S := (B^a % N) | (B^x % N)      S := (A^b % N) | (v^b % N) */
         
void bspeke (char *uname, char *pwd, uint64_t a, 
  uint64_t A, uint64_t b, uint64_t B, uint64_t N, uint64_t g)
{
  uint64_t S1, S2, x, v;
  
  printf ("\n  ---------------------------");
  
  // normally we'd use a cryptographic hash algorithm
  // for user name, password and random number
  x  = hash (uname);
  x += hash (pwd);
  x += rand_range (N);  // salt is constant, just ensures v is unique
  
  // v := g^x % N
  v=powmod (g, x, N);
  
  // we're using addition here but you should use cryptographic hash
  // S := (B^a % N) | (B^x % N)
  S1  = powmod (B, a, N);
  S1 += powmod (B, x, N);
  
  // we're using addition here but you should use cryptographic hash
  // S := (A^b % N) | (v^b % N)
  S2  = powmod (A, b, N);
  S2 += powmod (v, b, N);
  
  printf ("\n\n  Alice S=%llu", S1);
  printf ("\n  Bob   S=%llu", S2);
  
  // S1 and S2 should be equal
  printf ("\n\n  BSPEKE Key exchange %s\n", 
    S1 == S2 ? "succeeded" : "failed");
}

/**
   For WSPEKE:
         u := hash(B) / (2^128)              u := hash(B) / (2^128)
         S := B^(a + u*x) % N                  S := (A * v^u)^b % N */
         
void wspeke (char *uname, char *pwd, uint64_t a, 
  uint64_t A, uint64_t b, uint64_t B, uint64_t N, uint64_t g)
{
  uint64_t S1, S2, x, u, v;
  
  printf ("\n  ---------------------------");
  
  // x is derivation of user name, password and random salt
  x  = hash (uname);
  x += hash (pwd);
  x += rand_range (N);     // salt is constant, ensuring v is unique
  
  // Server only stores v
  // v := g^x % N
  v=powmod (g, x, N);
  
  // Client performs the following
  // u := hash(B) / (2^128)
  u=B;
  // S := B^(a + u*x) % N
  S1=powmod (B, add(a, multiply (u, x)), N); 
  
  // Server performs the following
  // u  := hash(B) / (2^128)
  u=B;
  // S2 := (A * v^u)^b % N
  S2=powmod (multiply (A, powmod (v, u, N)), b, N);
  
  printf ("\n\n  Alice S=%llu", S1);
  printf ("\n  Bob   S=%llu", S2);
  
  // S1 and S2 should be equal
  printf ("\n\n  WSPEKE Key exchange %s\n", 
    S1 == S2 ? "succeeded" : "failed");
}

int main (int argc, char *argv[])
{
  uint64_t h, p, g, a=0, b=0, A, B;
  char *pwd, *uname;
  
  puts ("\n  SPEKE examples");
  
  if (argc != 3) {
    printf ("\n  usage: speke <username> <password>\n");
    return 0;
  }
  
  pwd=argv[1];
  uname=argv[2];
  
  // generate safe prime N
  p=safe_prime (RP);
   
  // g = (H(pwd) * H(pwd)) mod p
  h=hash (pwd);
  g = sqrmod (h, p);
  
  printf ("\n  Prime        : %llu", p);
  printf ("\n  Generator    : %llu", g);
  
  // Alice and Bob agree to use same p and g for key exchange
  // Both generate a random integer to establish session key.
  a=rand_range (p);
  b=rand_range (p);
  
  // Alice computes g ^ a
  A=powmod (g, a, p);
  
  // Bob computes g ^ b
  B=powmod (g, b, p);
  /**
     a := random integer
     A := g^a % N
     A  ------------------------>
                                        b := random integer
                                               B := g^b % N
                             <--------------------------  B */
  // Alice and Bob exchange A and B values
  speke (a, A, b, B, p);
  bspeke (uname, pwd, a, A, b, B, p, g);
  wspeke (uname, pwd, a, A, b, B, p, g);

  return 0;
}


