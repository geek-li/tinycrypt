Chaskey Cipher

    Article: Chaskey: An Efficient MAC Algorithm for 32-bit Microcontrollers, SAC'14[7]
    Authors: Nicky Mouha, Bart Mennink, Anthony Van Herrewege, Dai Watanabe, Bart Preneel and Ingrid Verbauwhede
    Target: Software 

Chaskey is a lightweight MAC algorithm optimised for 32-bit micro-controllers. It is based on a 128-bit block cipher, the Chaskey cipher, which uses ARX operations and an Even-Mansour structure. This means that there is no key schedule: the 128-bit master key is XOR-ed, then a public permutation is applied and then the master key is XOR-ed again. This simplicity is possible at the cost of a weaker security claim as in e.g. PRINCE or PRIDE because a generic attack exists with a time complexity of about 2128/D if the attacker obtains D plaintext-ciphertext pairs.

The code implementing it is very simple and is given below. It is similar to that of SipHash.

The original paper also suggests doubling the number of rounds of the Chaskey cipher to obtain an even more secure primitive, Chaskey-LTS (Long Time Support), with 16 rounds. It was later suggested[101], in reaction to Leurent's differential-linear attack[102], to use a variant with 12 rounds called Chaskey-12. 