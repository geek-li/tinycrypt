

#include "binmath.h"

#define RP 31

short isqrt(short num) {
    short res = 0;
    short bit = 1 << 14; // The second-to-top bit is set: 1 << 30 for 32 bits
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;
        
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else
            res >>= 1;
        bit >>= 2;
    }
    return res;
}

// 16-bit 0x4000
// 32-bit 0x4000 0000
// 64-bit 0x4000 0000 0000 0000
uint64_t isqrt64(uint64_t num, uint64_t x) {
    uint64_t res = 0;
    uint64_t bit = x-2; //1ULL << 62; // The second-to-top bit is set: 1 << 30 for 32 bits
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;
        
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else
            res >>= 1;
        bit >>= 2;
    }
    return res;
}

// returns x where (a * x) % b == 1
uint64_t mul_inv(uint64_t a, uint64_t b)
{
	uint64_t b0 = b, t, q;
	uint64_t x0 = 0, x1 = 1;
  
	if (b == 1) return 1;
	
  while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if ((int64_t)x1 < 0) x1 += b0;
	return x1;
}
 
uint64_t chinese_remainder(uint64_t *n, uint64_t *a, uint64_t len)
{
	uint64_t p, i, prod = 1, sum = 0;
 
	for (i = 0; i < len; i++) prod *= n[i];
 
	for (i = 0; i < len; i++) {
		p = prod / n[i];
		sum += a[i] * mul_inv(p, n[i]) * p;
	}
 
	return sum % prod;
}

int main(int argc, char *argv[])
{
  uint64_t c, m, p, q, r, s, n, x, y, z;
  uint64_t x0[4]={3,5,7,9}, x1[4]={2,3,2,3};
  
  if (argc!=2) {
    printf ("\nusage: rabin <message>\n");
    return 0;
  }
  
  // generate private keys
  p=7; //safe_prime(RP);
  q=11; //safe_prime(RP);
  
  // calculate public key
  // n=pq
  n=multiply(p, q);
  
  // get message
  m=45; //hash(argv[1]);
  
  // ensure message is modulo n
  //m=modulo(m, n);
  
  // encrypt message
  // c=(m^2) mod n
  c=powmod(m, 2, n);
  
  // calculate square roots mod p, mod q
  //r=(c*((p+1) / 4) % p);
  //s=(c*((q+1) / 4) % q);
  
  // squared ciphertext
  r=(c*c) % p;
  // cubed ciphertext
  s=(c*c*c) % q;
  
  printf ("\n  CRT            = %llu", 
    chinese_remainder(x0, x1, 4));
  
  printf ("\n  Prime p        = %llu", p);
  printf ("\n  Prime p        = %llu", q);
  printf ("\n  Encryption Key = %llu", n);
  printf ("\n  Message        = %llu", m);
  printf ("\n  Encrypted Msg  = %llu", c);
  printf ("\n  r              = %llu", r);
  printf ("\n  s              = %llu", s);
  
  //printf ("\n  x              = %llu", x);
  //printf ("\n  y              = %llu", y);
  //printf ("\n  z              = %llu", z);
  
  return 0;
}
