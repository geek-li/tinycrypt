#include "led.h"

/* encrypt */
void encrypt(STATE *s, KEY *k) {

  int r;

  char kf = 0; /* key flag: use k_1 or k_2 */

  for (r = 0; r < RN; r++) {
    if ((r%4)==0) {
      addKey(s,k,kf);
      kf ^= 1;
    }

    addConstants(s,r);
    subCells(s);
    shiftRows(s); 
    mixColumnsSerial(s);
  }
  addKey(s,k,0);
}

/* addKey */
void addKey(STATE *s, KEY *k, char flag) {

  if (flag == 1 && KEYSIZE == 128) {
    s->b[0] ^= k->w[2]; s->b[1] ^= k->w[3];
  } else {
    s->b[0] ^= k->w[0]; s->b[1] ^= k->w[1];
  }
}

/* addConstants */
void addConstants(STATE *s, int r) {
  s->b[0] ^=               (((RCONST[r] >> 3) & 0x7) << 24) ^ (0x1 << 12) ^ (((RCONST[r] >> 0) & 0x7) << 8);
  s->b[1] ^= (0x2 << 28) ^ (((RCONST[r] >> 3) & 0x7) << 24) ^ (0x3 << 12) ^ (((RCONST[r] >> 0) & 0x7) << 8);
}

/* subCells */
void subCells(STATE *s) {
  int i;
  WORD x[2] = {0,0};
  /* apply SBox */
  for (i = 0; i < 8; i++) {
    x[0] ^= SBox[(s->b[0] >> (28-4*i)) & 0xf] << (28-4*i);
    x[1] ^= SBox[(s->b[1] >> (28-4*i)) & 0xf] << (28-4*i);
  } 
  s->b[0] = x[0]; 
  s->b[1] = x[1];
}

/* shiftRows */
void shiftRows(STATE *s) {
  s->b[0] = ((ROL16((s->b[0] >> 16) & 0xffff,0) & 0xffff) << 16) ^ (ROL16((s->b[0] >> 0) & 0xffff, 4) & 0xffff);
  s->b[1] = ((ROL16((s->b[1] >> 16) & 0xffff,8) & 0xffff) << 16) ^ (ROL16((s->b[1] >> 0) & 0xffff,12) & 0xffff);
}

/* mix columns */
void mixColumnsSerial(STATE *s) {
  int i;
  WORD x[2] = {0,0};
  BYTE t[4] = {0,0,0,0};

  /* iterate over columns */
  for (i = 0; i < 4; i++) {
    /* extract column */
    t[0] = (s->b[0] >> (28-4*i)) & 0xf;
    t[1] = (s->b[0] >> (12-4*i)) & 0xf;
    t[2] = (s->b[1] >> (28-4*i)) & 0xf;
    t[3] = (s->b[1] >> (12-4*i)) & 0xf;

    /* multiply matrix and column */
    x[0] ^= (gm(t[0],MDS[0][0]) ^ gm(t[1],MDS[0][1]) ^ gm(t[2],MDS[0][2]) ^ gm(t[3],MDS[0][3])) << (28-4*i);
    x[0] ^= (gm(t[0],MDS[1][0]) ^ gm(t[1],MDS[1][1]) ^ gm(t[2],MDS[1][2]) ^ gm(t[3],MDS[1][3])) << (12-4*i);
    x[1] ^= (gm(t[0],MDS[2][0]) ^ gm(t[1],MDS[2][1]) ^ gm(t[2],MDS[2][2]) ^ gm(t[3],MDS[2][3])) << (28-4*i);
    x[1] ^= (gm(t[0],MDS[3][0]) ^ gm(t[1],MDS[3][1]) ^ gm(t[2],MDS[3][2]) ^ gm(t[3],MDS[3][3])) << (12-4*i);
  }
  s->b[0] = x[0]; s->b[1] = x[1];
}

/* galois multiplication in GF(16) */
BYTE gm(BYTE a, BYTE b) {
  BYTE g = 0;
  BYTE hbs;
  int i;
  for (i = 0; i < DEG_GF_POLY; i++) {
    if ( (b & 0x1) == 1 ) { g ^= a; }
    hbs = (a & 0x8);
    a <<= 0x1;
    if ( hbs == 0x8) { a ^= GF_POLY; }
    b >>= 0x1;
  }
  return g;
}

/*
int main (int argc, char *argv[]) {

  STATE s;
  KEY k;

  int i = 0;

  while ((i = getopt(argc, argv, "k:p:")) >= 0) {
    switch (i) {
    case 'k':
      if (strlen(optarg) == 17 && KEYSIZE == 64) {
        k.w[0] = strtoul(strtok(optarg," "),NULL,16);  
        k.w[1] = strtoul(strtok(NULL," "),NULL,16);  
      } else if (strlen(optarg) == 35 && KEYSIZE == 128) {
        k.w[0] = strtoul(strtok(optarg," "),NULL,16);  
        k.w[1] = strtoul(strtok(NULL," "),NULL,16);  
        k.w[2] = strtoul(strtok(NULL," "),NULL,16);  
        k.w[3] = strtoul(strtok(NULL," "),NULL,16);
      } else {
        printf("Error! Wrong key length.\n"); 
        return EXIT_FAILURE;
      }
      break;
    case 'p':
      if (strlen(optarg) == 17) {
        s.b[0] = strtoul(strtok(optarg," "),NULL,16); 
        s.b[1] = strtoul(strtok(NULL," "),NULL,16);
      } else {
        printf("Error! Wrong size of plaintext block.\n"); 
        return EXIT_FAILURE;
      }
      break;
    default:
      return EXIT_FAILURE;
    }
  }
  encrypt(&s,&k);
  printf("%08x %08x\n", s.b[0], s.b[1]);
  return EXIT_SUCCESS;
}
*/