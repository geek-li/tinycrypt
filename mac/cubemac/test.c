

#include "cm.h"
#include "cubehash.h"

int main(int argc, char *argv[])
{
  hashState    state;
  crypto_uint8 mkey[64], mac[16]; 
  int          i, bits;
  char         *key, *msg;
  
  if (argc != 3) {
    printf ("\ntest <key> <message>\n");
    return 0;
  }
  
  key = argv[1];
  msg = argv[2];
  
  // create key first
  Init(&state, 512);
  Update(&state, key, strlen(key) * 8);
  Final(&state, mkey);  
  
  // now the MAC
  Init(&state, 128);
  Update(&state, mkey, 512);
  Update(&state, msg, strlen(msg) * 8);
  Final(&state, mac); 
  
  for (i=0; i<16; i++) {
    printf ("%02x", mac[i]);
  }
  
  putchar('\n');
  
  //cube_setkey(&ctx, mkey);
  cube_macx(mkey, 64, msg, strlen(msg), mac);
  
  for (i=0; i<16; i++) {
    printf ("%02x", mac[i]);
  }  
  return 0;
}
