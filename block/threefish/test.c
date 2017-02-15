
// test unit for threefish
// odzhan

#include <stdio.h>
#include <string.h>

#include "threefish.h"

uint64_t three_256_00_key[]    = { 0L, 0L, 0L, 0L };
uint64_t three_256_00_input[]  = { 0L, 0L, 0L, 0L };
uint64_t three_256_00_tweak[]  = { 0L, 0L };
uint64_t three_256_00_result[] = { 0x94EEEA8B1F2ADA84L, 0xADF103313EAE6670L,
                                   0x952419A1F4B16D53L, 0xD83F13E63C9F6B11L };
                                   
                                   
uint64_t three_256_01_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
                                0x2726252423222120L, 0x2F2E2D2C2B2A2928L};

uint64_t three_256_01_input[] = { 0xF8F9FAFBFCFDFEFFL, 0xF0F1F2F3F4F5F6F7L,
                                  0xE8E9EAEBECEDEEEFL, 0xE0E1E2E3E4E5E6E7L};

uint64_t three_256_01_result[] = 
{ 0xDF8FEA0EFF91D0E0ULL,  0xD50AD82EE69281C9ULL,
  0x76F48D58085D869DULL,  0xDF975E95B5567065ULL };

uint64_t three_256_01_tweak[] = 
{ 0x0706050403020100L, 0x0F0E0D0C0B0A0908L };
                          
typedef struct _threefish_tv {
  uint64_t *key;
  uint64_t *input;
  uint64_t *tweak;
  uint64_t *result;
}tf_tv;
  
tf_tv tv[2]=
{{three_256_00_key, three_256_00_input, three_256_00_tweak, three_256_00_result},
 {three_256_01_key, three_256_01_input, three_256_01_tweak, three_256_01_result}};  
 
void print_bytes(char *s, void *p, int len)
{
  int i;
  printf ("\n%s = ", s);
  for (i=0; i<len; i++) {
    if ((i & 31)==0) putchar('\n');
    printf ("%02x ", ((uint8_t*)p)[i]);
  }
}
 
int main(void)
{
  threefish_ctx_t ctx;
  int i;
  uint8_t t[32];
  
  for (i=0; i<2; i++)
  {
    memcpy (t, tv[i].input, 32);
    
    //print_bytes("Key", tv[i].key, 32);
    //print_bytes("Input", tv[i].input, 32);
    //print_bytes("Tweak", tv[i].tweak, 16);
    
    memset (&ctx, 0, sizeof (ctx));
    //printf ("\n\nSetting up key for test # %i", (i+1));
    threefish_setkeyx (&ctx, tv[i].key, tv[i].tweak);
    //print_bytes("CTX", &ctx, sizeof(ctx));
    
    //printf ("\nEncrypting plaintext");
    threefish_encryptx(&ctx, tv[i].input, THREEFISH_ENCRYPT);
  
    //print_bytes("Ciphertext", tv[i].input, 32);
    //print_bytes("Expected", tv[i].result, 32);
  
    if (memcmp(tv[i].input, tv[i].result, 32)==0) {
      printf ("\nEncryption OK");
    } else printf("\nEncryption failed");
  
    threefish_encryptx(&ctx, tv[i].input, THREEFISH_DECRYPT);
  
    //print_bytes("Plaintext", tv[i].input, 32);
    //print_bytes("Expected", t, 32);
    
    if (memcmp(t, tv[i].input, 32)==0) {
      printf ("\nDecryption OK");
    } else printf("\nDecryption failed");
  }
  return 0;
}