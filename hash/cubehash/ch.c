/*
20081110
D. J. Bernstein
Public domain.
*/

#include "parameters.h"
#include "cubehash.h"

#define ROTATE(a,b) (((a) << (b)) | ((a) >> (32 - b)))

static void transform(cube_ctx *c)
{
  int      i, j, k;
  int      r;
  uint32_t y[16];

  for (r=0; r<CUBEHASH_ROUNDS; ++r) 
  {
    for (k=7, j=8; j>0; k+=4, j-=4)
    {
      for (i=0; i<16; ++i) c->x[i + 16] += c->x[i];
      for (i=0; i<16; ++i) y[i ^ j] = c->x[i];
      for (i=0; i<16; ++i) c->x[i] = ROTATE(y[i], k);
      for (i=0; i<16; ++i) c->x[i] ^= c->x[i + 16];
      for (i=0; i<16; ++i) y[i ^ (j>>2)] = c->x[i + 16];
      for (i=0; i<16; ++i) c->x[i + 16] = y[i];
    }
  }
}

HashReturn Init(cube_ctx *c, uint32_t mdlen)
{
  int i, j;

  if (mdlen < 8) return BAD_HASHBITLEN;
  if (mdlen > 512) return BAD_HASHBITLEN;
  if (mdlen != ((mdlen >> 3) << 3)) return BAD_HASHBITLEN;

  c->mdlen = mdlen;
  
  for (i=0; i<32; ++i) {
    c->x[i] = 0;
  }
  c->x[0] = mdlen >> 3;
  c->x[1] = CUBEHASH_BLOCKBYTES;
  c->x[2] = CUBEHASH_ROUNDS;
  
  for (i=0; i<10; ++i) {
    transform(c);
  }
  c->pos = 0;
  return SUCCESS;
}

void Update(cube_ctx *c, const void *in, uint32_t inlen)
{
  uint8_t  *data=(uint8_t*)in;
  uint32_t u, r, i;
  
  /* caller promises us that previous data had integral number of bytes */
  /* so c->pos is a multiple of 8 */

  while (inlen>0)
  {
    // add byte to state
    u=*data++;
    u <<= (((c->pos >> 3) & 3) << 3);
    c->x[c->pos >> 5] ^= u;
    r = (inlen>(CUBEHASH_BLOCKBYTES<<3)) ? (CUBEHASH_BLOCKBYTES<<3) : inlen;
    c->pos += r;
    inlen -= r;
    // transform
    if (c->pos == (CUBEHASH_BLOCKBYTES<<3)) {
      transform(c);
      c->pos=0;
    }
  }
}

void Final(cube_ctx *c, void *out)
{
  int      i;
  uint32_t u;

  u = (128 >> (c->pos & 7));
  
  u <<= (((c->pos >> 3) & 3) << 3);
  c->x[c->pos >> 5] ^= u;
  
  transform(c);
  
  // add the end bit
  c->x[31] ^= 1;
  
  // apply final rounds
  for (i=0; i<10; ++i) {
    transform(c);
  }
  // return hash
  for (i = 0;i < c->mdlen >> 3;++i) 
    ((uint8_t*)out)[i] = c->x[i];
}
