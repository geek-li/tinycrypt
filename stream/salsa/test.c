
// test unit for salsa20-256 with 64-bit nonce
// odzhan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s20.h"

char tv_key[]="0F62B5085BAE0154A7FA4DA0F34699EC"
              "3F92E5388BDE3184D72A7DD02376C91C";

char tv_iv[] ="288FF65DC42B92F9";

char tv_res[]="5E5E71F90199340304ABB22A37B6625B"
              "F883FB89CE3B21F54A10B81066EF87DA"
              "30B77699AA7379DA595C77DD59542DA2"
              "08E5954F89E40EB7AA80A84A6176663F";
    
int equ(uint8_t x[], uint8_t y[], int len) {
    return memcmp(x, y, len)==0;
}

uint32_t hex2bin (void *bin, char hex[]) {
    uint32_t len, i;
    uint32_t x;
    uint8_t *p=(uint8_t*)bin;
    
    len = strlen (hex);
    
    if ((len & 1) != 0) {
      return 0; 
    }
    
    for (i=0; i<len; i++) {
      if (isxdigit((int)hex[i]) == 0) {
        return 0; 
      }
    }
    
    for (i=0; i<len / 2; i++) {
      sscanf (&hex[i * 2], "%2x", &x);
      p[i] = (uint8_t)x;
    } 
    return len / 2;
} 

void bin2hex(void *in, int len) {
    int i;
    uint8_t *p=(uint8_t*)in;
    
    for (i=0; i<len; i++) {
      if ((i & 7)==0) putchar('\n');
      printf ("%02x, ", p[i]);
    }
    putchar('\n');
}

int main(void)
{
    s20_ctx c;
    uint8_t strm[64], res[64], key[32], iv[16];
    
    memset(strm, 0, sizeof(strm));
    
    hex2bin(key, tv_key);
    hex2bin(iv, tv_iv);
    hex2bin(res, tv_res);
    
    s20_setkey(&c, key, iv);
    s20_cryptx(&c, strm, sizeof(strm));
    
    printf ("\nSALSA20 test - %s", 
      equ(strm, res, 64) ? "OK" : "failed");
        
    bin2hex(strm, 64);
    return 0;
}
