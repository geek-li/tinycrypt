#ifndef LED_H_
#define LED_H_

#include <stdio.h>
#include <string.h>
//#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define LED 128

typedef unsigned char BYTE; /* 8 bit */
typedef unsigned long WORD; /* 32 bit */

#if     LED==64
#define KEYSIZE         64 /* 64-bit key */
#define N                2
#define RN              32
#elif   LED==128
#define KEYSIZE        128 /* 128-bit key */
#define N                4
#define RN              48
#endif

#define BLOCK_SIZE      64

#define GF_POLY       0x13 /* irreducible polynomial x^4 + x + 1 for GF(16) */
#define DEG_GF_POLY      4

#define ROL16(x,n) (((x) << ((n) & 0xf)) | ((x) >> (16-((n) & 0xf) )))
#define ROR16(x,n) (((x) >> ((n) & 0xf)) | ((x) << (16-((n) & 0xf) )))

/* 64-/ 128-bit key*/
typedef struct {
  WORD w[N]; 
} KEY;

/* 64-bit state */
typedef struct {
  WORD b[2]; 
} STATE;

/* function definitions */
void encrypt(STATE *s, KEY *k);
void addKey(STATE *s, KEY *k, char flag);
void addConstants(STATE *s, int r);
void subCells(STATE *s);
void shiftRows(STATE *s);
void mixColumnsSerial(STATE *s);
BYTE gm(BYTE a, BYTE b);

/* round constants */
const BYTE RCONST[48] = {
  0x01,0x03,0x07,0x0F,
  0x1F,0x3E,0x3D,0x3B,
  0x37,0x2F,0x1E,0x3C,
  0x39,0x33,0x27,0x0E,
  0x1D,0x3A,0x35,0x2B,
  0x16,0x2C,0x18,0x30,
  0x21,0x02,0x05,0x0B,
  0x17,0x2E,0x1C,0x38,
  0x31,0x23,0x06,0x0D,
  0x1B,0x36,0x2D,0x1A,
  0x34,0x29,0x12,0x24,
  0x08,0x11,0x22,0x04
};

/* the LED SBox */
BYTE SBox[16] = { 
  0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD,
  0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2
};

/* the matrix for MixColumnsSerial */
const BYTE MDS[4][4] = {
  {0x4,0x1,0x2,0x2},
  {0x8,0x6,0x5,0x6},
  {0xB,0xE,0xA,0x9},
  {0x2,0x2,0xF,0xB},
};


#endif /* LED_H_ */
