

#include "cubehash.h"

int main(int argc, char *argv[])
{
  hashState    state;
  crypto_uint8 hash[64]; 
  int          i, bits;
  
  if (argc!=3) {
    printf ("\ntest <bits> <message>\n");
    return 0;
  }
  
  bits = atoi(argv[1]);
  
  Init(&state, bits);
  Update(&state, argv[2], strlen(argv[2]) * 8);
  Final(&state, hash);  
  
  for (i=0; i<bits/8; i++) {
    printf ("%02x", hash[i]);
  }
  return 0;
}
