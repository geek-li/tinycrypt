


// Password Authenticated Key Exchange by Juggling
// RFC XXXX

#include "binmath.h"

#define RN 32 // for creating modulus

typedef struct
{
  uint64_t p;
  uint64_t q;
  uint64_t g;
} JPakeParameters;
    
typedef struct
{
  uint64_t gr;  // g^r (r random)
  uint64_t b;   // b = r - x*h, h=hash(g, g^r, g^x, name)
} JPakeZKP;

typedef struct
{
  uint64_t gx;       // g^x
  JPakeZKP zkpx;    // ZKP(x)
} JPakeStep1;

typedef struct
{
  uint64_t X;        // g^(xa + xc + xd) * xb * s
  JPakeZKP zkpxbs;  // ZKP(xb * s)
} JPakeStep2;

typedef struct
{
   char *name;  // Must be unique
  int base;          // 1 for Alice, 3 for Bob. Only used for printing stuff.
  JPakeStep1 s1c;    // Alice's g^x3, ZKP(x3) or Bob's g^x1, ZKP(x1)
  JPakeStep1 s1d;    // Alice's g^x4, ZKP(x4) or Bob's g^x2, ZKP(x2)
  JPakeStep2 s2;     // Alice's A, ZKP(x2 * s) or Bob's B, ZKP(x4 * s)
} JPakeUserPublic;
    
/*
 * The user structure. In the definition, (xa, xb, xc, xd) are Alice's
 * (x1, x2, x3, x4) or Bob's (x3, x4, x1, x2). If you see what I mean.
 */
typedef struct
{
  JPakeUserPublic p;
  uint64_t secret;    // The shared secret
  uint64_t key;       // The calculated (shared) key
  uint64_t xa;        // Alice's x1 or Bob's x3
  uint64_t xb;        // Alice's x2 or Bob's x4
} JPakeUser;
    
void JPakeParametersInit (JPakeParameters *params)
{
  uint64_t p, q, g, t;
  
  p=safe_prime (RN);
  q=(p-1)/2;
  
  for (g=1; g == 1;) {
    // g = t ^ (p-1)/q % p
    t=rand_range (q);            // normally, t would be a hash of q bits
    g=powmod (t, (p-1)/q, p);
  }
  params->p=p;
  params->q=q;
  params->g=g;
  
  // For now use p, q, g, but generate later randomly
  // generated same as DSA domain parameters
  /*
  params->p = hex2bin("5E6DDA8BC9B2B5A9");
  params->q = hex2bin("D549");
  params->g = hex2bin("22729DB49EE21F4B"); */
}

void genrand(JPakeUser *user, JPakeParameters *params)
{
  uint64_t qm1;

  // xa in [0, q)
  user->xa = rand_range(params->q);
  // q-1
  qm1 = params->q - 1;

  // ... and xb in [0, q-1)
  user->xb = rand_range(params->q-1);
  // [1, q)
  user->xb += 1;
  
  // Show
  printf ("x%d", user->p.base);
  printf (" %s", bin2hex(user->xa));
  printf ("\nx%d", user->p.base+1);
  printf (" %s\n", bin2hex(user->xb));
}
    
// h=hash(g, g^r, g^x, name)
uint64_t zkpHash(JPakeZKP *zkp, uint64_t gx,
  JPakeUserPublic *from, JPakeParameters *params)
{
  /**
  uint8_t md[SHA_DIGEST_LENGTH];
  SHA_CTX sha;

  // XXX: hash should not allow moving of the boundaries - Java code
  // is flawed in this respect. Length encoding seems simplest.
  SHA1_Init(&sha);
  hashbn(&sha, params->g);
  hashbn(&sha, zkp->gr);
  hashbn(&sha, gx);
  hashstring(&sha, from->name);
  SHA1_Final(md, &sha);
  BN_bin2bn(md, SHA_DIGEST_LENGTH, h); */
  
  uint64_t h=0;
  h += params->g;
  h += zkp->gr;
  h += gx;
  h += hash (from->name);
  return h;
}
    
// Prove knowledge of x
// Note that we don't send g^x because, as it happens, we've always
// sent it elsewhere. Also note that because of that, we could avoid calculating it here, but we don't, for clarity... 
void CreateZKP(JPakeZKP *zkp, uint64_t x, JPakeUser *us,
  uint64_t zkpg, JPakeParameters *params)
{
  uint64_t r, gx, h, t;
  
  // r in [0,q)
  // XXX: Java chooses r in [0, 2^160) - i.e. distribution not uniform
  r=rand_range(params->q);
  // g^r
  zkp->gr = powmod(zkpg, r, params->p);
  // g^x
  gx = powmod(zkpg, x, params->p);

  // h=hash...
  h=zkpHash(zkp, gx, &us->p, params);

  // b = r - x*h
  t = mulmod(x, h, params->q);
  zkp->b = submod(r, t, params->q);
}

int VerifyZKP(JPakeZKP *zkp, uint64_t x, 
  JPakeUserPublic *them, uint64_t zkpg, 
  JPakeParameters *params)
{
  uint64_t h, t1, t2, t3;

  h=zkpHash(zkp, x, them, params);

  // t1 = g^b
  t1=powmod(zkpg, zkp->b, params->p);
  // t2 = (g^x)^h = g^{hx}
  t2=powmod(x, h, params->p);
  // t3 = t1 * t2 = g^{hx} * g^b = g^{hx+b} = g^r (allegedly)
  t3=mulmod(t1, t2, params->p);

  /*printf ("  ZKP(x%d%s)\n", n, suffix);
  printf ("    zkpg %s", bin2hex(zkpg));
  printf ("    g^r' %llX != %llX", t3, zkp->gr); */

  // verify t3 == g^r
  return (t3 == zkp->gr);
} 

void sendstep1_substep(JPakeStep1 *s1,  uint64_t x, JPakeUser *us, JPakeParameters *params)
{
  s1->gx = powmod(params->g, x, params->p);
  /*printf ("  g^{x%d}", n);
  printf ("%s", bin2hex(s1->gx));*/

  CreateZKP(&s1->zkpx, x, us, params->g, params);
}

void sendstep1( JPakeUser *us, JPakeUserPublic *them, JPakeParameters *params)
{
  printf ("\n%s sends %s:\n\n", us->p.name, them->name);

  // from's g^xa (which becomes to's g^xc) and ZKP(xa)
  sendstep1_substep (&them->s1c, us->xa, us, params);
  // from's g^xb (which becomes to's g^xd) and ZKP(xb)
  sendstep1_substep (&them->s1d, us->xb, us, params);
}

int verifystep1( JPakeUser *us,  JPakeUserPublic *them, JPakeParameters *params)
{
  printf ("\n%s verifies %s:\n\n", us->p.name, them->name);

  // verify their ZKP(xc)
  if (!VerifyZKP(&us->p.s1c.zkpx, us->p.s1c.gx, them, params->g, params)) {
    printf ("\nZKP (xc) failed");
    return 0;
  }

  // verify their ZKP(xd)
  if(!VerifyZKP(&us->p.s1d.zkpx, us->p.s1d.gx, them, params->g, params)) {
    printf ("\nZKP (xd) failed");
    return 0;
  }
  // g^xd != 1
  //printf ("  g^{x%d}", them->base+1);
  if (us->p.s1d.gx == 1)
  {
    puts("FAIL");
    return 0;
  }
  puts("OK");
  return 1;
}
    
void sendstep2(JPakeUser *us, JPakeUserPublic *them, JPakeParameters *params)
{
  uint64_t t1, t2;
  
  printf("\n%s sends %s:\n\n", us->p.name, them->name);

  // X = g^{(xa + xc + xd) * xb * s}
  // t1 = g^xa
  t1=powmod(params->g, us->xa, params->p);
  // t2 = t1 * g^{xc} = g^{xa} * g^{xc} = g^{xa + xc}
  t2=mulmod(t1, us->p.s1c.gx, params->p);
  // t1 = t2 * g^{xd} = g^{xa + xc + xd}
  t1=mulmod(t2, us->p.s1d.gx, params->p);
  // t2 = xb * s
  t2=mulmod(us->xb, us->secret, params->q);
  // X = t1^{t2} = t1^{xb * s} = g^{(xa + xc + xd) * xb * s}
  them->s2.X=powmod(t1, t2, params->p);

  // Show
  /*printf("  g^{(x%d + x%d + x%d) * x%d * s)", us->p.base, them->base,
   them->base+1, us->p.base+1);
  printf("%s", bin2hex (them->s2.X));*/

  // ZKP(xb * s)
  // XXX: this is kinda funky, because we're using
  //
  // g' = g^{xa + xc + xd}
  //
  // as the generator, which means X is g'^{xb * s}
  CreateZKP (&them->s2.zkpxbs, t2, us, t1, params);
}
    
int verifystep2( JPakeUser *us,  JPakeUserPublic *them,
		        JPakeParameters *params)
{
  uint64_t t1, t2;
  
  printf("\n%s verifies %s:\n\n", us->p.name, them->name);

  // g' = g^{xc + xa + xb} [from our POV]
  // t1 = xa + xb
  t1=addmod(us->xa, us->xb, params->q);
  // t2 = g^{t1} = g^{xa+xb}
  t2=powmod(params->g, t1, params->p);
  // t1 = g^{xc} * t2 = g^{xc + xa + xb}
  t1=mulmod(us->p.s1c.gx, t2, params->p);

  return VerifyZKP(&us->p.s2.zkpxbs, us->p.s2.X, them, t1, params);
}

void computekey(JPakeUser *us,  JPakeParameters *params)
{
  uint64_t t1, t2, t3;
  
  printf ("\n%s calculates the shared key :", us->p.name);

  // K = (X/g^{xb * xd * s})^{xb}
  //   = (g^{(xc + xa + xb) * xd * s - xb * xd *s})^{xb}
  //   = (g^{(xa + xc) * xd * s})^{xb}
  //   = g^{(xa + xc) * xb * xd * s}
  // [which is the same regardless of who calculates it]

  // t1 = (g^{xd})^{xb} = g^{xb * xd}
  t1=powmod (us->p.s1d.gx, us->xb, params->p);
  // t2 = -s = q-s
  t2=subtract (params->q, us->secret);
  // t3 = t1^t2 = g^{-xb * xd * s}
  t3=powmod (t1, t2, params->p);
  // t1 = X * t3 = X/g^{xb * xd * s}
  t1=mulmod (us->p.s2.X, t3, params->p);
  // K = t1^{xb}
  us->key=powmod (t1, us->xb, params->p);

  // show
  printf ("  K=%s", bin2hex(us->key));
}

int main (int argc, char *argv[])
{
  JPakeParameters params;
  JPakeUser alice, bob;
  char *pwd;
  
  puts ("\n  JPAKE example");
  
  if (argc != 2) {
    printf ("\n  usage: jpake <password>\n");
    return 0;
  }
  pwd=argv[1];
  
  alice.p.name = "Alice";
  alice.p.base = 1;
  
  bob.p.name   = "Bob";
  bob.p.base   = 3;
  
  JPakeParametersInit (&params);
  
  // password / shared secret
  alice.secret=modulo (hash(pwd), params.q);
  bob.secret=alice.secret;
  
  genrand (&alice, &params);
  genrand (&bob, &params);
  
  sendstep1 (&alice, &bob.p, &params);
  sendstep1 (&bob, &alice.p, &params);
  
  // And verify what each other sent
  if(!verifystep1(&alice, &bob.p, &params)) {
    printf ("\n  Alice Verify 1 failed");
    return 1;
  }
  if(!verifystep1(&bob, &alice.p, &params)) {
    printf ("\n  Bob Verify 1 failed");
    return 2;
  }

  // Second send
  sendstep2(&alice, &bob.p, &params);
  sendstep2(&bob, &alice.p, &params);

  // And second verify
  if(!verifystep2(&alice, &bob.p, &params)) {
    printf ("\n  Alice Verify 2 failed");
    return 3;
  }
  if(!verifystep2(&bob, &alice.p, &params)) {
    printf ("\n  Bob Verify 2 failed");
    return 4;
  }

  // Compute common key
  computekey(&alice, &params);
  computekey(&bob, &params);
    
  return 0;
}
