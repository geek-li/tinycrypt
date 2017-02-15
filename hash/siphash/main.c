#include "siphash.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>


int main(void)
{
  uint8_t key[16], msg[16], res[8]={0};
  int  i, j;
  
  for (i=0; i<16; i++) {
    key[i] = i;
    msg[i] = i;
  }
  
  for (i=0; i<8; i++) {
    siphash24(res, msg, sizeof(msg) - i, key);

    printf("\nSipHash-2-4 test # %i: ", (i+1));
  
    for (j=0; j<8; ++j) {
      printf("0x%02x ", res[j]);
    }
  }
  return 0;
}

