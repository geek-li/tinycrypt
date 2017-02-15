

                            Binmath v0.1
                            ------------
              
[ intro

Most cryptographic protocols rely upon modular arithmetic using large 
numbers in excess of 1024-bits to provide reasonable security when 
exchanging keys between 2 parties over an insecure channel. 

While researching various protocols and schemes; key exchange, signing 
and verification. I thought it would be nice to have miniature functions 
to perform all the arithmetic but only for 64-bit numbers. 

Consider this a toy for those who want to understand crypto algorithms 
that depend on modular arithmetic. 


[ examples

Below are some simple examples using binmath.

[ ***************************
[ Diffie Hellman
[ ***************************

  Diffie-Hellman-Merkle Key Exchange example

  Generating p...7667183
  Generating g...3389256

  Alice S=1908601
  Bob   S=1908601

  Key exchange succeeded

[ ***************************
[ Digital Signature Algorithm
[ ***************************

  DSA example

  Generating p...1EFB837
  Generating g...99F470

p=1EFB837
q=F7DC1B
g=99F470

public key = BCED3C
Private key = 73DEE3

(k=2EA207, r=8EF1C, s=2C7043)
v = 8EF1C
  signature is valid
  
  
[ ***************************
[ ElGamal
[ ***************************

  ElGamal Encryption example

  Generating p...
  Generating g...

  Prime          : 3119
  Generator      : 2071
  Message        : "message"
  Hash           : 1452

  B: Shared secret  : 2515
  B: Plaintext Hash : 1452
  B: Encrypted Hash : 2116 2550
  A: Shared secret  : 2515
  A: Decrypted Hash : 1452

  Decryption succeeded
  
[ ***************************
[ JPAKE (key exchange)
[ ***************************
  JPAKE example
x1 88AA70
x2 668B05
x3 57298D
x4 8E75D7

Alice sends Bob:
Bob sends Alice:
Alice verifies Bob:
OK
Bob verifies Alice:
OK
Alice sends Bob:
Bob sends Alice:
Alice verifies Bob:
Bob verifies Alice:

Alice calculates the shared key :  K=6EC252
Bob calculates the shared key :  K=6EC252

[ ***************************
[ RSA Signing/Verification
[ ***************************

  RSA example

  Generating p...14703383
  Generating q...5077643

  modulus        : 74658529766269
  message        : "message"
  encryption key : 65537
  decryption key : 49156802483685
  signature      : 53018284977315 (good)

[ ***************************
[ SPEKE
[ ***************************

  SPEKE examples

  Prime        : 12434207
  Generator    : 3639259
  ---------------------------

  Alice S=11758194
  Bob   S=11758194

  SPEKE Key exchange succeeded

  ---------------------------

  Alice S=20933654
  Bob   S=20933654

  BSPEKE Key exchange succeeded

  ---------------------------

  Alice S=8921343
  Bob   S=8921343

  WSPEKE Key exchange succeeded
  
  
[ ***************************
[ Secure Remote Password
[ ***************************

  SRP Key Exchange example

  Generating N...21037607
  Generating g...20044909

  Client S=15364346
  Server S=15364346


  Key exchange succeeded
  