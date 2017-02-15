

#include <stdint.h>
#include <stdio.h>

// do not use this for real application
// initialize with more appropriate source of information!
// time() is not random but you should know that already..
void fill_rand (uint8_t out[], int len) {
  int i;
  srand(time(0));
  for (i=0; i<len; i++) {
    out[i] = (uint8_t)rand();
  }
}

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */

void xtea_enc (uint32_t rnds, void *data, void *key) 
{
  uint32_t *v=(uint32_t*)data;
  uint32_t *k=(uint32_t*)key;
  
  uint32_t i;
  uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
  
  for (i=0; i<rnds; i++) {
      v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
      sum += delta;
      v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
  }
  v[0]=v0; 
  v[1]=v1;
}

void xtea_dec (uint32_t rnds, void *data, void *key) 
{
  uint32_t *v=(uint32_t*)data;
  uint32_t *k=(uint32_t*)key;
  
  uint32_t i;
  uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*rnds;
  
  for (i=0; i<rnds; i++) {
      v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
      sum -= delta;
      v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
  }
  v[0]=v0; 
  v[1]=v1;
}

void dump (char txt[], uint8_t in[], int len) {
  int i;
  printf ("\n%s : ", txt);
  for (i=0; i<len; i++) {
    printf ("%02x", in[i]);
  }
}

int main (int argc, char *argv[])
{
  uint8_t key[16];
  uint8_t text[8];

  fill_rand(key, 16);
  fill_rand(text, 8);
  
  dump ("plaintext", text, 8);
  
  xtea_enc (32, text, key);
  dump ("encrypted", text, 8);
  
  xtea_dec (32, text, key);
  dump ("decrypted", text, 8);
  return 0;
}
