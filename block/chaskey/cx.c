/*
   Chaskey-12 reference C implementation

   Written in 2015 by Nicky Mouha, based on Chaskey

   To the extent possible under law, the author has dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
   
   NOTE: This implementation assumes a little-endian architecture
         that does not require aligned memory accesses.
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define memcpy(x,y,z) __movsb(x,y,z)
#define memset(x,y,z) __stosb(x,y,z)

#define ROTL(x,b) (uint32_t)( ((x) >> (32 - (b))) | ( (x) << (b)) )

void chas_permute(uint32_t v[])
{
  int i;
  for (i=0; i<12; i++)
  {
    v[0] += v[1]; \
    v[1]=ROTL32(v[1], 5); \
    v[1] ^= v[0]; \
    v[0]=ROTL32(v[0],16); \
    v[2] += v[3]; \
    v[3]=ROTL32(v[3], 8); \
    v[3] ^= v[2]; \
    v[0] += v[3]; \
    v[3]=ROTL32(v[3],13); \
    v[3] ^= v[0]; \
    v[2] += v[1]; \
    v[1]=ROTL32(v[1], 7); \
    v[1] ^= v[2]; \
    v[2]=ROTL32(v[2],16); \
  }
}
  
const volatile uint32_t C[2] = { 0x00, 0x87 };

void TIMESTWO(uint32_t out[], const uint32_t in[]) 
{
    out[0] = (in[0] << 1) ^ C[in[3] >> 31];
    out[1] = (in[1] << 1) | (in[0] >> 31); 
    out[2] = (in[2] << 1) | (in[1] >> 31); 
    out[3] = (in[3] << 1) | (in[2] >> 31); 
}
    
void chas_setkey(void *out, void *in) 
{
  uint32_t *key=(uint32_t*)out;
  
  memcpy (key, in, 16);
  
  TIMESTWO(&key[4], &key[0]);
  TIMESTWO(&key[8], &key[4]);
}

void chas_xor(void *out, const void *in, int len) {
  int i;

  for (i=0; i<len; i++) {
    ((uint8_t*)out)[i] ^= ((uint8_t*)in)[i];
  }
}
  
void chas_mac (
    uint8_t *tag, 
    uint32_t taglen, 
    uint8_t *msg, 
    uint32_t msglen,
    uint8_t  *key) 
{
  const uint32_t *l;
  uint8_t lb[16];
  uint32_t v[4];
  
  int i, len;
  
  // copy 16 bytes of key
  memcpy(v, &key[0], 16);

  // absorb message 
  for (;;)
  {
    len = (msglen < 16) ? msglen : 16;
    
    // copy available data to local buffer
    memcpy(lb, msg, len);
    
    // final? add padding bit
    if (len<16) {
      lb[len++] = 0x01;
    }
    
    // xor state with data
    chas_xor(v, lb, len);

    // final?
    if (msglen<=16) {
      key += (msglen==16) ? 16 : 32;
      break;
    }    
    
    // apply permutation function
    chas_permute(v);
    
    // update position and length
    msg += len;
    msglen -= len;
  }

  // pre-whiten
  chas_xor(v, key, 16);
  // permute
  chas_permute(v);
  // post-whiten
  chas_xor(v, key, 16);

  // return tag
  memcpy(tag, v, taglen);
}

#ifdef TEST
const uint32_t vectors[64][4] =
{
  { 0x43CB1F41, 0x51EBA0C2, 0xFF0A8AC3, 0x7EE3F642 },
  { 0xF9AC2067, 0x9C35A846, 0x441AAD3D, 0x777B7330 },
  { 0x57DA70C5, 0x2A873CB0, 0x19EE8B2A, 0x165CD82E },
  { 0x8C5E6AB9, 0x5035ADFB, 0xBFF69F98, 0x965516D9 },
  { 0x0B2B62DB, 0x1E9E3F50, 0xA1B8DCAD, 0xB4279AE0 },
  { 0x39FA92B9, 0x1B655E4F, 0x5E4A4667, 0x0FE13365 },
  { 0x7C814DEC, 0x149F38A0, 0x270046B9, 0xFB954C27 },
  { 0xB7D29CB8, 0x40A2819D, 0xAE403CDB, 0x6FBEFA95 },
  { 0x9FAF57D6, 0xF4BC02CF, 0x6AF6D831, 0xD2930D90 },
  { 0x8417124D, 0x552889A7, 0x35D716F0, 0xE04632A6 },
  { 0xDEA5BA76, 0x741D87ED, 0x72CFEF1A, 0x91749FC9 },
  { 0x6A888831, 0x8679ED53, 0x8A192E58, 0x58B23BD1 },
  { 0xC040258C, 0xF25392C0, 0x9F6B5DC0, 0x35C3D638 },
  { 0x7FEBA9C3, 0x585DA8E9, 0x7680BE51, 0x9FB8FC6E },
  { 0xC133C9C0, 0x55DF75B5, 0x0F18F729, 0x99B9837E },
  { 0x03CFB44B, 0x283C8163, 0xFCA71448, 0xC40A0AEA },
  { 0x5DD0E2A9, 0xFB5EAC8C, 0x633A392E, 0x500C36F3 },
  { 0x8B5F6D5A, 0x202314F6, 0x22092368, 0x9639E606 },
  { 0x0430889E, 0xB994DC9C, 0xA39D8D46, 0xF0B15FDA },
  { 0x426CA8DD, 0x9DA954C9, 0x613290FC, 0x9AEBE7E9 },
  { 0xECE3BDB9, 0x17E5A589, 0x64AA2EEF, 0x9A75ECED },
  { 0xEFDDD3D7, 0xBE458309, 0xD430468E, 0x44C17D41 },
  { 0x440809BA, 0x87C9512B, 0xE495C3B6, 0x3601D81D },
  { 0x0B1DB893, 0x05300791, 0x5E789C3B, 0x4BBE102A },
  { 0x9F01C148, 0xAE4FC446, 0x6563E38E, 0xD5483B99 },
  { 0x9AC00551, 0x80C778DA, 0xD894CE35, 0x56598BCB },
  { 0x3ABB0B87, 0x3E0FBB0E, 0x7635D502, 0xC913C4EC },
  { 0xC5CFBFFF, 0x6C52AE42, 0x025D402A, 0xC154FCE1 },
  { 0x76D1EB98, 0x8C72085A, 0x77155006, 0xBF389002 },
  { 0x7CB65A88, 0x5E7C9B65, 0xD5C24284, 0xBD64DEFE },
  { 0xC082B077, 0x8E22EA68, 0xBFD34969, 0x0418D7DA },
  { 0x1D3D30B1, 0x01D74FCA, 0x3B2EB54C, 0xD8CD36B4 },
  { 0x77962784, 0x07C647FA, 0xDD752C0F, 0xD2F5A799 },
  { 0xD74BB867, 0x2C0DFB3A, 0xB697E9CD, 0x5658EDDA },
  { 0x4CDDF615, 0xEF51F2F3, 0xB254B4AE, 0xFDAB76D9 },
  { 0x7567E1CA, 0xF2379487, 0x75082E35, 0x463F164D },
  { 0xA98ECB80, 0x5EF583CF, 0x4E20E76E, 0x7873B8F1 },
  { 0x1446E9AA, 0x1BE1EC9B, 0x1D475DE5, 0xA82C5D00 },
  { 0x11D3A094, 0xC43D33FA, 0xD33D6C42, 0xD6604682 },
  { 0x3B09C785, 0x867BEA15, 0x1E05031D, 0xBC4C8072 },
  { 0x155AB3AB, 0x73D51ED7, 0xAC6F3601, 0xEF6AA85C },
  { 0x28E86031, 0xDCEB8B32, 0x63B1D172, 0xA4B65AC7 },
  { 0xF02660F5, 0x6EB38FF8, 0x6AF8730C, 0x0694B77C },
  { 0x3A3806A1, 0x54161686, 0x437B82F2, 0x541CDC9D },
  { 0x4BDFFF32, 0xB258591A, 0x26AD161B, 0xE3445E89 },
  { 0xEEDEBC62, 0xE9F9DE19, 0x252E8047, 0x553411A2 },
  { 0xA7AEBC31, 0xC10AD12E, 0x85B25FA0, 0x6DD54E7F },
  { 0x8494B5BC, 0x5317B7B3, 0xCFE08756, 0x97A2D14E },
  { 0xF36E748B, 0x8F34677F, 0x1E00BA1B, 0xDD7DA46D },
  { 0xF9F4055F, 0xAF76AC88, 0x45DA034C, 0xF1C04C8A },
  { 0x6F486CFD, 0x72653E7D, 0xE597059C, 0x03A1580D },
  { 0x54080FC5, 0xC9E98B24, 0x3C9EDE0F, 0x79CAB6BA },
  { 0x4EC246AA, 0x01EDAAB3, 0xBFE09C48, 0x7C5C4C45 },
  { 0xFFD828F3, 0xE8875C0C, 0x18CE432D, 0xC42DA43A },
  { 0x9C45CFF1, 0x1A38A387, 0xA7FBCB03, 0x41649EF5 },
  { 0x31C29F70, 0xD6E4DD76, 0x03562D6F, 0x902E3DF6 },
  { 0x9AB66191, 0x7DAF7DFF, 0x868090AE, 0x35B7D6C6 },
  { 0x4D569CCA, 0x7F53FED2, 0x16525A72, 0xFAB67A70 },
  { 0x08EC0D1E, 0xC96855B8, 0xEE9E3842, 0xDD3C6CD6 },
  { 0xE70DFFB1, 0x74FAD311, 0xEF723024, 0xB6C2B5C6 },
  { 0xA8F07A68, 0x366FC33B, 0xAF00EEFB, 0x1A48A9DF },
  { 0x079E0279, 0xF6C252F7, 0xE99E3E03, 0x88BA1A2A },
  { 0x0F40ED0F, 0xA69BC80F, 0x8E06F97C, 0x61E89697 },
  { 0x2DD072E1, 0x42B89EFE, 0xFB26B615, 0x049AA451 }
};

int test_vectors() {
  uint8_t m[64];
  uint8_t tag[16];
  uint32_t k[4] = 
{ 0x833D3433, 0x009F389F, 0x2398E64F, 0x417ACF39 };

  uint8_t key[48]; 
  int i;
  int ok = 1;
  uint32_t taglen = 16;

  // key schedule 
  chas_setkey(key, k);

  // test vectors for mac function 
  for (i = 0; i < 64; i++) {
    m[i] = i;
    
    chas_mac(tag, taglen, m, i, key);

    if (memcmp( tag, vectors[i], taglen )) {
      printf("test vector failed for %d-byte message\n", i);
      ok = 0;
    }
  }

  return ok;
}

int main() {
  if (test_vectors()) printf("test vectors ok\n");

  return 0;
}
#endif
