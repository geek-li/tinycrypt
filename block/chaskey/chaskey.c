#include <stdint.h>
#define ROTL(x,b) (uint32_t)( ((x) >> (32 - (b))) | ( (x) << (b)) )

void encrypt(uint32_t v[4], uint32_t key[4]) {
   int i;
   for (i=0; i<4; i++) v[i] ^= key[i];
   for (i=0; i<8; i++) {
     v[0] += v[1]; v[1]=ROTL(v[1], 5); v[1] ^= v[0]; v[0]=ROTL(v[0],16);
     v[2] += v[3]; v[3]=ROTL(v[3], 8); v[3] ^= v[2];
     v[0] += v[3]; v[3]=ROTL(v[3],13); v[3] ^= v[0];
     v[2] += v[1]; v[1]=ROTL(v[1], 7); v[1] ^= v[2]; v[2]=ROTL(v[2],16);
   }
   for (i=0; i<4; i++) v[i] ^= key[i];
}