/**
  Copyright © 2016 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include "kuznechik.h"

// The S-Box from section 5.1.1
const uint8_t kuz_pi[0x100] = {
  0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16,   // 00..07
  0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,   // 08..0F
  0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA,   // 10..17
  0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,   // 18..1F
  0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21,   // 20..27
  0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,   // 28..2F
  0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0,   // 30..37
  0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,   // 38..3F
  0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB,   // 40..47
  0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,   // 48..4F
  0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12,   // 50..57
  0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,   // 58..5F
  0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7,   // 60..67
  0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,   // 68..6F
  0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E,   // 70..77
  0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,   // 78..7F
  0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9,   // 80..87
  0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,   // 88..8F
  0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC,   // 90..97
  0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,   // 98..9F
  0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44,   // A0..A7
  0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,   // A8..AF
  0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F,   // B0..B7
  0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,   // B8..BF
  0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7,   // C0..C7
  0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,   // C8..CF
  0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE,   // D0..D7
  0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,   // D8..DF
  0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B,   // E0..E7
  0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,   // E8..EF
  0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0,   // F0..F7
  0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6,   // F8..FF
};

// initialize sbox tables
void kuz_init(kuz_key_t *key)
{
  int i;

  for (i=0; i<256; i++) {
    key->pi[i] = kuz_pi[i];
    key->pi_inv[kuz_pi[i]] = i;
  }
}

// poly multiplication mod p(x) = x^8 + x^7 + x^6 + x + 1
// totally not constant time
static uint8_t kuz_mul_gf256(uint8_t x, uint8_t y)
{
  uint8_t z;

  z = 0;

  while (y) {
    if (y & 1) {
      z ^= x;
    }
    x = (x << 1) ^ (x & 0x80 ? 0xC3 : 0x00);
    y >>= 1;
  }
  return z;
}

static void kuz_lt (w128_t *w, int enc) {
  int     i, j;
  uint8_t x;

  // Linear vector from sect 5.1.2
  const uint8_t kuz_lvec[16] = {
    0x94, 0x20, 0x85, 0x10, 0xC2, 0xC0, 0x01, 0xFB,
    0x01, 0xC0, 0xC2, 0x10, 0x85, 0x20, 0x94, 0x01 };

  // 16 rounds
  for (j=0; j<16; j++)
  {
    if (enc == KUZ_ENCRYPT)
    {
      // An LFSR with 16 elements from GF(2^8)
      x = w->b[15]; // since lvec[15] = 1

      for (i = 14; i >= 0; i--) {
        w->b[i + 1] = w->b[i];
        x ^= kuz_mul_gf256(w->b[i], kuz_lvec[i]);
      }
      w->b[0] = x;
    } else {
      x = w->b[0];

      for (i = 0; i < 15; i++) {
        w->b[i] = w->b[i + 1];
        x ^= kuz_mul_gf256(w->b[i], kuz_lvec[i]);
      }
      w->b[15] = x;
    }
  }
}

// key whitening
static void kuz_whiten(w128_t *w, kuz_key_t *key, int idx) {
  int i;
  uint32_t *p=(uint32_t*)&key->k[idx].b[0];
  
  for (i=0; i<4; i++) {
    w->w[i] ^= p[i];
  }
}

// substitute bytes
static void kuz_subbytes(w128_t *w, kuz_key_t *key, int enc) {
  int           i;
  const uint8_t *sbox = (enc==KUZ_ENCRYPT) ? key->pi : key->pi_inv;

  for (i=0; i<16; i++) {
    w->b[i] = sbox[w->b[i]];
  }
}

// key setup
void kuz_setkey(kuz_key_t *kuz, const uint8_t key[32])
{
  int    i, j;
  w128_t c, z;
  w256_t x;

  // copy key to local buffer
  memcpy (&x.b[0], key, 32);
  // copy key to context
  memcpy (&kuz->k[0].b[0], &x.b[0], 32);

  for (i = 1; i <= 32; i++) {

    // C Value
    __stosd (&c.b[0], 0, 4);
    //c.q[ 0] = 0;
    //c.q[ 1] = 0;
    c.b[15] = i;    // load round in lsb

    kuz_lt(&c, KUZ_ENCRYPT);

    for (j=0; j<4; j++) {
      z.w[j] = x.w[j] ^ c.w[j];
    }

    kuz_subbytes(&z, kuz, KUZ_ENCRYPT);
    kuz_lt(&z, KUZ_ENCRYPT);

    for (j=0; j<16; j++) {
      z.b[j] ^= x.b[16+j];
    }
    //z.q[0] ^= x.q[2];
    //z.q[1] ^= x.q[3];

    //memcpy (&x.b[16], &x.b[0], 16);
    x.q[2] = x.q[0];
    x.q[3] = x.q[1];

    x.q[0] = z.q[0];
    x.q[1] = z.q[1];

    if ((i & 7) == 0) {
      kuz->k[(i >> 2)    ].q[0] = x.q[0];
      kuz->k[(i >> 2)    ].q[1] = x.q[1];

      kuz->k[(i >> 2) + 1].q[0] = x.q[2];
      kuz->k[(i >> 2) + 1].q[1] = x.q[3];
    }
  }
}

// encrypt/decrypt a block - 8 bit way
void kuz_encrypt(kuz_key_t *key, void *blk, int enc)
{
  int    i, j;
  w128_t *x=(w128_t*)blk;

  if (enc==KUZ_ENCRYPT)
  {
    i=0;
    for (;;)
    {
      kuz_whiten(x, key, i);
      if (++i == KUZ_ROUNDS) break;
      kuz_subbytes(x, key, enc);
      kuz_lt(x, enc);
    }
  } else {
    i=KUZ_ROUNDS;
    for (;;)
    {
      i--;
      kuz_whiten(x, key, i);
      if (i == 0) break;
      kuz_lt(x, enc);
      kuz_subbytes(x, key, enc);
    }
  }
}

