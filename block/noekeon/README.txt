
Noekeon Block Cipher

Designers         Joan Daemen, Michaël Peeters,
                  Gilles Van Assche, Vincent Rijmen
First published   2000-09
Derived from      3-Way, BaseKing
Key sizes         128 bits
Block sizes       128 bits
Rounds            16

Noekeon is a Substitution-Permutation Network operating on blocks of 128 
bits using a 128-bits key. It operates on 4 words of 32 bits except for 
the S-Box layer, "Gamma", which operates on 4-bits nibbles. The same 
round key is used in every round; how it is derived depends on whether 
related-key attacks must be considered or not. However, there exists 
related-key differentials for both key schedules[96] It uses the 
following operations. 

Gamma: Consists in applying a 4-bit involution S-Box on nibbles 
independently. Each of the 32 nibbles considered in Gamma is made of the 
bits of index i in each of the 4 words for all i in [0, 31]. This leads 
to a simple bitslice implementation of this layer. Most choices for 
Gamma generated using the same design criteria would have lead to weak 
ciphers but the one chosen in Noekeon does not[96]. Theta: A linear 
layer which mixes words with each other and operates at the byte level. 
It has a Lai-Massey structure where the Lai-Massey function is linear: x 
\mapsto x \oplus (x <<< 8) \oplus (x >>> 8). The round key is XOR-ed 
between the 2-steps of the Lai-Lassey operation. shift operations: Three 
of the four words are rotated by different offsets, namely 1, 5 and 2. 
Each rotations and their inverses are used. 

A round constant is XOR-ed in the internal state before applying Gamma 
during encryption. Since the components are involution-based, decryption 
can be implemented using the same circuit as encryption. 16 rounds are 
used. 

It is claimed to be suitable for implementation in hardware and on 8-bit 
processors. 

The best attack by the designers is a linear attack based on a 2-rounds 
iterative linear trail covering 9 rounds, which is then extended to 
cover 12 rounds through key guessing. 

