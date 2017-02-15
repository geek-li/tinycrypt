

 [ TEA Block cipher

Designers         Roger Needham, David Wheeler
First published   1994
Successors        XTEA
Key sizes         128 bits
Block sizes       64 bits
Structure         Feistel network
Rounds            variable; recommended 64 Feistel rounds (32 cycles)


 [ XTEA Block cipher

Designers         Roger Needham, David Wheeler
First published   1997
Derived from      TEA
Successors        Corrected Block TEA
Key sizes         128 bits
Block sizes       64 bits
Structure         Feistel cipher
Rounds            variable; recommended 64 Feistel rounds (32 cycles)


 [ XXTEA Block cipher
 
Designers         David Wheeler, Roger Needham
First published   1998
Derived from      Block TEA
Key sizes         128 bits
Block sizes       arbitrary, at least two words (64 bits)
Structure         Unbalanced Feistel Network
Rounds            depends on the block size; ~52+6*words (6-32 full cycles)


TEA in x86 assembly

I've tried to verify the correctness of the algorithm and ran into problems with test
vectors taken off the internet from various random sources.

To my knowledge, the implementation here is fine.

tea.c          - reference and my own implementation of encrypt/decrypt functions.
test.c         - simple unit to test tea.c

tea_crypt1.asm - 2 separate functions in x86 assembly
tea_crypt2.asm - 2 separate functions in x86 assembly that use carry flag for looping
tea_crypt3.asm - 1 function that uses carry flag and jmp operation for encrypt/decrypt
tea_crypt.asm  - 1 function that uses carry flag and negate operation for ADD/SUB