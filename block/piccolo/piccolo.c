
#include "piccolo.h"

/*
 * x contains the user submitted key:
 *  80-bit: k_0 = x[0] | x[1],...,k_4 = x[ 8] | x[ 9]
 * 128-bit: k_0 = x[0] | x[1],...,k_8 = x[14] | x[15]
 */
void keySchedule(BYTE x[], KEY *k) {
  int i, r, c;
  BYTE y[N];
    
  /* init whitening keys */
  k->wKey[0] = 0x0;
  k->wKey[1] = 0x0;

  /* init round keys */

  for (i = 0; i < RN; i++) {
    k->rKey[i] = 0x0;
  }

  /* compute keys */
  if (KEYSIZE == 80) {

    /* set whitening keys */
    k->wKey[0] ^= (x[0] << 24);
    k->wKey[0] ^= (x[3] << 16);
    k->wKey[0] ^= (x[2] <<  8);
    k->wKey[0] ^= (x[1] <<  0);
   
    k->wKey[1] ^= (x[8] << 24);
    k->wKey[1] ^= (x[7] << 16);
    k->wKey[1] ^= (x[6] <<  8);
    k->wKey[1] ^= (x[9] <<  0);

    /* set round keys */
    for (r = 0; r < RN; r++) {
 
      /* generates the constants */
      //k->rKey[r] ^= ((r+1) << 27) ^ ((r+1) << 17) ^ ((r+1) << 10) ^ ((r+1) << 0) ^ 0x0F1E2D3C;
      //printf("%08x\n", k->rKey[r]);
      
      /* use precomputed constants */
      k->rKey[r] ^= C[r];

      if ( r%5 == 0 || r%5 == 2 ) {
        k->rKey[r] ^= (x[4] << 24) ^ (x[5] << 16) ^ (x[6] << 8) ^ (x[7] << 0);
      } else if ( r%5 == 1 || r%5 == 4 ) {
        k->rKey[r] ^= (x[0] << 24) ^ (x[1] << 16) ^ (x[2] << 8) ^ (x[3] << 0);
      } else if (r%5 == 3) {
        k->rKey[r] ^= (x[8] << 24) ^ (x[9] << 16) ^ (x[8] << 8) ^ (x[9] << 0);
      }
    }
   
  } else if (KEYSIZE == 128) {

    /* set whitening keys */
    k->wKey[0] ^= (x[ 0] << 24);
    k->wKey[0] ^= (x[ 3] << 16);
    k->wKey[0] ^= (x[ 2] <<  8);
    k->wKey[0] ^= (x[ 1] <<  0);
   
    k->wKey[1] ^= (x[ 8] << 24);
    k->wKey[1] ^= (x[15] << 16);
    k->wKey[1] ^= (x[14] <<  8);
    k->wKey[1] ^= (x[ 9] <<  0);
  
    /* init buffer */ 
    for (r = 0; r < N; r++) {
      y[r] = x[r]; 
    }

    /* generates the constants
    for (r = 0; r < RN; r++) {
      k->rKey[r] ^= ((r+1) << 27) ^ ((r+1) << 17) ^ ((r+1) << 10) ^ ((r+1) << 0)  ^ 0x6547A98B;
      printf("%08x\n", k->rKey[r]);
    }
    */
    
    for (r = 0; r < 2*RN; r++) {
      c = (r+2)%8;      
      if (c == 0) {
        y[ 0] = x[ 4]; y[ 1] = x[ 5]; // k_0 = k_2
        y[ 2] = x[ 2]; y[ 3] = x[ 3]; // k_1 = k_1
        y[ 4] = x[12]; y[ 5] = x[13]; // k_2 = k_6
        y[ 6] = x[14]; y[ 7] = x[15]; // k_3 = k_7
        y[ 8] = x[ 0]; y[ 9] = x[ 1]; // k_4 = k_0
        y[10] = x[ 6]; y[11] = x[ 7]; // k_5 = k_3
        y[12] = x[ 8]; y[13] = x[ 9]; // k_6 = k_4
        y[14] = x[10]; y[15] = x[11]; // k_7 = k_5
   
        /* update x */
        for (i = 0; i < N; i++) {
          x[i] = y[i]; 
        }
      }
      if (r%2 == 0) {
        k->rKey[r/2] ^= (C[r/2] & 0xffff0000) ^ (y[2*c] << 24) ^ (y[2*c+1] << 16);
      } else {
        k->rKey[r/2] ^= (C[r/2] & 0x0000ffff) ^ (y[2*c] << 8) ^ (y[2*c+1] << 0);
      }
    }
  }
}



void encrypt(STATE *s, KEY *k) {
  BYTE x[2] = {0,0};
  int r = 0;
  
  /* Debug */
  //printf("WKEY:    %08x\n", k->wKey[0]);
  
  /* input whitening */
  s->b[0] ^= (k->wKey[0] & 0xffff0000);       // X_0 ^= wk_0
  s->b[1] ^= (k->wKey[0] & 0x0000ffff) << 16; // X_2 ^= wk_1

  for (r = 0; r < RN; r++) {

    /* Debug */
    /*
      printf("Round %d\n",r);
      printf("START:   %08x %08x\n", s->b[0], s->b[1]);
      printf("RKEYS:   %08x\n", k->rKey[r]);
    */

    /* add round key rk_2i to X_1 */
    s->b[0] ^= (k->rKey[r] >> 16) & 0xffff;

    /* Debug */
    //printf("X_1+RK:  %08x %08x\n", s->b[0], s->b[1]);

    /* extract X_0, apply f function and add result to X_1 */
    x[0] = (s->b[0] >> 24) & 0xff;
    x[1] = (s->b[0] >> 16) & 0xff;
    f(x);
    s->b[0] ^= x[0] << 8;
    s->b[0] ^= x[1] << 0;

    /* Debug */
    //printf("UP(X_1): %08x %08x\n", s->b[0], s->b[1]);

    /* add round key rk_2i to X_3 */
    s->b[1] ^= (k->rKey[r] >>  0) & 0xffff;
    
    /* Debug */
    //printf("X_3+RK:  %08x %08x\n", s->b[0], s->b[1]);

    /* extract X_2, apply f function and add result to X_3 */
    x[0] = (s->b[1] >> 24) & 0xff;
    x[1] = (s->b[1] >> 16) & 0xff;
    f(x);
    s->b[1] ^= x[0] << 8;
    s->b[1] ^= x[1] << 0;

    /* Debug */
    //printf("UP(X_3): %08x %08x\n", s->b[0], s->b[1]);

    /* apply round permutation */
    if (r != RN-1) {
      rp(s);
      /* Debug */
      //printf("RP(S):   %08x %08x\n", s->b[0], s->b[1]);
    }
    //printf("\n");
  }

  /* Debug */
  //printf("WKEY:    %08x\n", k->wKey[1]);

  /* output whitening */
  s->b[0] ^= (k->wKey[1] & 0xffff0000);       // X_0 ^= wk_2
  s->b[1] ^= (k->wKey[1] & 0x0000ffff) << 16; // X_2 ^= wk_3
}


void f(BYTE b[]) {

  /* Debug */
  //printf("x:       %02x %02x\n", b[0], b[1]);
  
  /* init buffers */
  BYTE x[4] = {0,0,0,0};
  BYTE y[4] = {0,0,0,0};

  /* apply SBox */
  x[0] = SBox[(b[0] >> 4) & 0xf];
  x[1] = SBox[(b[0] >> 0) & 0xf];
  x[2] = SBox[(b[1] >> 4) & 0xf];
  x[3] = SBox[(b[1] >> 0) & 0xf];

  /* Debug */
  //printf("SBox(x): %x%x %x%x\n", x[0], x[1], x[2], x[3]);

  /* multiply matrix and column */
  y[0] = (gm(x[0],M[0][0]) ^ gm(x[1],M[0][1]) ^ gm(x[2],M[0][2]) ^ gm(x[3],M[0][3]));
  y[1] = (gm(x[0],M[1][0]) ^ gm(x[1],M[1][1]) ^ gm(x[2],M[1][2]) ^ gm(x[3],M[1][3]));
  y[2] = (gm(x[0],M[2][0]) ^ gm(x[1],M[2][1]) ^ gm(x[2],M[2][2]) ^ gm(x[3],M[2][3]));
  y[3] = (gm(x[0],M[3][0]) ^ gm(x[1],M[3][1]) ^ gm(x[2],M[3][2]) ^ gm(x[3],M[3][3]));

  /* Debug */
  //printf("M*x:     %x%x %x%x\n", y[0], y[1], y[2], y[3]);

  /* apply SBox */
  x[0] = SBox[y[0]];
  x[1] = SBox[y[1]];
  x[2] = SBox[y[2]];
  x[3] = SBox[y[3]];

  /* update array */
  b[0] = (x[0] << 4) ^ x[1];
  b[1] = (x[2] << 4) ^ x[3];

  /* Debug */
  //printf("f(x):    %02x %02x\n", b[0], b[1]);
}


void rp(STATE *s) {

  WORD t[2] = {0,0}; /* buffer */

  /* split, permute, reassemble 64-bit state */
  t[1] ^= ((s->b[0] >> 24) & 0xff) <<  8; // x_0 -> x_6
  t[0] ^= ((s->b[0] >> 16) & 0xff) <<  0; // x_1 -> x_3
  t[0] ^= ((s->b[0] >>  8) & 0xff) << 24; // x_2 -> x_0
  t[1] ^= ((s->b[0] >>  0) & 0xff) << 16; // x_3 -> x_5

  t[0] ^= ((s->b[1] >> 24) & 0xff) <<  8; // x_4 -> x_2
  t[1] ^= ((s->b[1] >> 16) & 0xff) <<  0; // x_5 -> x_7
  t[1] ^= ((s->b[1] >>  8) & 0xff) << 24; // x_6 -> x_4
  t[0] ^= ((s->b[1] >>  0) & 0xff) << 16; // x_7 -> x_1

  s->b[0] = t[0]; s->b[1] = t[1];
}


/* galois multiplication in F_16 */
BYTE gm(BYTE a, BYTE b) {
  BYTE hbs, g = 0;
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

// 8d2bff99 35f84056
// 5ec42cea 657b89ff
int main (int argc, char *argv[]) {

  // init state
  STATE s;
  KEY k;
  // key buffer
  BYTE x[N]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
             0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
  WORD w;
  int i = 0;
  uint8_t p[]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
 
  memcpy((uint8_t*)&s, p, 8);
  
  s.b[0] = _byteswap_ulong(s.b[0]);
  s.b[1] = _byteswap_ulong(s.b[1]);
  
  printf("M:       %08x %08x\n", s.b[0], s.b[1]);
  printf("KEY:     ");
  for (i=0; i<N; i++) {if(i%4==0 && i!=0){printf(" ");} printf("%02x",x[i]);}
  printf("\n");

  // init key
  keySchedule(x,&k);

  encrypt(&s,&k);
  printf("C:       %08x %08x\n", s.b[0], s.b[1]);

  return EXIT_SUCCESS;
}
