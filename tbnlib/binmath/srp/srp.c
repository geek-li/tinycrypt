


// Secure Remote Password Protocol
// RFC 5054

#include "binmath.h"

#define RN 32 // for creating modulus

int main (int argc, char *argv[])
{
  uint64_t a, b, q, t, v, g, x, A, B, u, k, t1, t2, S1, S2, N;
  char *pwd, *uname;
  
  puts ("\n  SRP Key Exchange example");
  
  if (argc != 3) {
    printf ("\n  usage: srp <username> <password>\n");
    return 0;
  }
  pwd=argv[1];
  uname=argv[2];
  /**
   The group parameters (N, g) sent in this message MUST have N as a
   safe prime (a prime of the form N=2q+1, where q is also prime).  The
   integers from 1 to N-1 will form a group under multiplication % N,
   and g MUST be a generator of this group.  In addition, the group
   parameters MUST NOT be specially chosen to allow efficient
   computation of discrete logarithms. */
   
  printf ("\n  Generating N...");
  
  // generate safe prime N
  N=safe_prime (RN);
  
  printf ("%llu",   N);
  
  printf ("\n  Generating g...");
  // create generator
  q=(N-1)/2;
  do {
    t=rand_range(N);
    g=powmod (t, (N-1)/q, N);
  } while (g <= 1);
  
  printf ("%llu\n", g);
  
  /** 
    The verifier (v) is computed based on the salt (s), user name (I),
    password (P), and group parameters (N, g).  The computation uses the
    [SHA1] hash algorithm:

        x = SHA1(s | SHA1(I | ":" | P))
        v = g ^ x % N
   */
  x = rand_range (N); // salt
  x += hash (uname);
  x += hash (pwd);
  x = modulo (x, N);     // just in case x > N (not used in practice)
  
  // Server stores verifier
  v=powmod (g, x, N);
  
  // both client and server calculate k
  k=N; k+=g;
  k=modulo (k, N);
  
  // both client and server calculate A and B, then exchange
  a=rand_range (N);
  // A = (g ^ a)
  A=powmod (g, a, N);
  
  b=rand_range (N);
  // B = (k * v) + (g ^ b)
  B=add (multiply (k, v), powmod (g, b, N));
  
  // both client and server calculate u using SHA-1
  u=A; u+=B;
  u=modulo (u, N);
  
  /**
    The pre master secret is calculated by the client as follows:

      I, P = <read from user>
      N, g, s, B = <read from server>
      a = random()
      A = g^a % N
      u = SHA1(PAD(A) | PAD(B))
      k = SHA1(N | PAD(g))
      x = SHA1(s | SHA1(I | ":" | P))
      <pre master secret> = (B - (k * g^x)) ^ (a + (u * x)) % N */
  
  /* S = (B - k * (g ^ x)) ^ (a + u * x) */
  t2=add (a, multiply (u, x));
  t1=subtract (B, multiply (k, powmod (g, x, N)));
  S1=powmod (t1, t2, N);
  
  printf ("\n  Client S=%llu", S1);
  
  /** 
    The pre master secret is calculated by the server as follows:

      N, g, s, v = <read from password file>
      b = random()
      k = SHA1(N | PAD(g))
      B = k*v + g^b % N
      A = <read from client>
      u = SHA1(PAD(A) | PAD(B))
      <pre master secret> = (A * (v ^ u)) ^ b % N */
  
  // S2 = (A * (v ^ u)) ^ b
  S2=powmod (multiply (A, powmod (v, u, N)), b, N);
  
  printf ("\n  Server S=%llu\n", S2);
  
  // S1 and S2 should be equal
  printf ("\n\n  Key exchange %s\n", 
    S1 == S2 ? "succeeded" : "failed");
  return 0;
}
