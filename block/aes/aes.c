
/**
  Copyright © 2015 Odzhan. All Rights Reserved.

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

#include "aes.h"

#define RotWord(x) ROTR32(x, 8)

uint32_t gf_mul2 (uint32_t w) {
  uint32_t t = w & 0x80808080;
  
  return ( (w ^ t ) << 1) ^ ( ( t >> 7) * 0x0000001B);
}

// ------------------------------------
// multiplicative inverse
// ------------------------------------
uint8_t gf_mulinv (uint8_t x)
{
  uint8_t y=x, i;

  if (x)
  {
    // calculate logarithm gen 3
    for (y=1, i=0; ;i++) {
      y ^= gf_mul2 (y);
      if (y==x) break;
    }
    i+=2;
    // calculate anti-logarithm gen 3
    for (y=1; i; i++) {
      y ^= gf_mul2(y);
    }
  }
  return y;
}
// ------------------------------------
// substitute byte
// ------------------------------------
uint8_t SubByte (uint8_t x, int enc)
{
  uint8_t i, y=0, sb;

  if (enc==AES_ENCRYPT)
  {
    sb = y = gf_mulinv (x);

    for (i=0; i<4; i++) {
      y   = ROTL8(y, 1);
      sb ^= y;
    }
    sb ^= 0x63;
  } else {
    // ------------------
    y   = x ^ 0x63;
    y   = ROTL8(y, 1);
    sb  = y;
    y   = ROTL8(y, 2);
    sb ^= y;
    y   = ROTL8(y, 3);
    sb ^= y;
    sb  = gf_mulinv(sb);
  }
  return sb;
}
// ------------------------------------
// substitute 4 bytes
// ------------------------------------
uint32_t SubWord (uint32_t x)
{
  uint8_t i;
  uint32_t r=0;

  for (i=0; i<4; i++) {
    r |= SubByte(x & 0xFF, AES_ENCRYPT);
    r  = ROTR32(r, 8);
    x >>= 8;
  }
  return r;
}
// ------------------------------------
// substitute 16 bytes
// ------------------------------------
void SubBytes (aes_blk *state, int enc)
{
  int8_t i;

  for (i=0; i<16; i++) {
    state->v8[i] = SubByte(state->v8[i], enc);
  }
}
// ------------------------------------
// shift rows in state
// ------------------------------------
void ShiftRows (aes_blk *state, int enc)
{
  uint32_t x, i, j;

  // shift 4 rows
  for (i=0; i<4; i++) {
    x=0;
    // get row
    for (j=i; j<16; j+=4) {
      x |= state->v8[j];
      x=ROTR32(x, 8);
    }
    // rotate depending on enc
    if (enc==AES_ENCRYPT) {
      x = ROTR32(x, i*8);
    } else {
      x = ROTL32(x, i*8);
    }
    // set row
    for (j=i; j<16; j+=4) {
      state->v8[j] = (x & 0xff);
      x >>= 8;
    }
  }
}

// ------------------------------------
// mix columns in state
// ------------------------------------
uint32_t MixColumn (uint32_t w) {
  return ROTR32(w, 8) ^ 
         ROTR32(w, 16) ^ 
         ROTR32(w, 24) ^ 
         gf_mul2(ROTR32(w, 8) ^ w);
}

void MixColumns (uint32_t *state, int enc)
{
  uint32_t i, t, w;

  for (i=0; i<4; i++)
  {
    w = state[i];
    if (enc==AES_DECRYPT) {
      t = ROTR32(w, 16) ^ w;
      t = gf_mul2(gf_mul2(t));
      w ^= t;
    }
    state[i] = MixColumn(w);
  }
}
// ------------------------------------
// add 16 bytes of key to state
// ------------------------------------
void AddRoundKey (aes_blk *state, uint32_t w[], int rnd)
{
  uint32_t i;
  uint8_t *key=(uint8_t*)&w[rnd*4];

  for (i=0; i<16; i++) {
    state->v8[i] ^= key[i];
  }
}
// ------------------------------------
// create AES-256 key
// ------------------------------------
void aes_setkey (aes_ctx *ctx, void *key)
{
  int i;
  uint32_t x;
  uint32_t *w=(uint32_t*)ctx->w;
  uint8_t rcon=1;

  for (i=0; i<Nk; i++) {
    w[i]=((uint32_t*)key)[i];
  }

  for (i=Nk; i<Nb*(Nr+1); i++)
  {
    x=w[i-1];
    if ((i % Nk)==0) {
      x = RotWord(x);
      x = SubWord(x) ^ rcon;
      rcon=gf_mul2(rcon);
    } else if (Nk > 6 && i % Nk == 4) {
      x=SubWord(x);
    }
    w[i] = w[i-Nk] ^ x;
  }
}
// ------------------------------------
// encrypt/decrypt 16 bytes of state
// ------------------------------------
void aes_encrypt (aes_ctx *ctx, void *state, int enc)
{
  uint8_t round;
  uint32_t *w=(uint32_t*)ctx->w;

  if (enc==AES_ENCRYPT)
  {
    AddRoundKey (state, w, 0);

    for (round=1; round<Nr; round++)
    {
      SubBytes (state, enc);
      ShiftRows (state, enc);
      MixColumns (state, enc);
      AddRoundKey (state, w, round);
    }
  }
  else
  {
    AddRoundKey (state, w, Nr);

    for (round=Nr-1; round>0; round--)
    {
      SubBytes (state, enc);
      ShiftRows (state, enc);
      AddRoundKey (state, w, round);
      MixColumns (state, enc);
    }
  }
  
  SubBytes (state, enc);
  ShiftRows (state, enc);
  AddRoundKey (state, w, round);
}
