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

#include "rabbit.h"

// Square a 32-bit unsigned integer to
// obtain the 64-bit result and return
// the upper 32 bits XOR the lower 32 bits
#ifndef _WIN64
__declspec(naked) uint32_t RABBIT_g_func(uint32_t x)
#else
uint32_t RABBIT_g_func(uint32_t x)
#endif
{
  uint32_t h;
   #ifndef _WIN64
   __asm {
     mov    eax, [esp+4]
     xor    edx, edx
     mul    eax
     xor    eax, edx
     ret
   };
   #elif _WIN64
   uint64_t sum = U64V(x) * x;

   h = U32V((sum >> 32) ^ (sum & 0xFFFFFFFF));
   return h;
   #else
   uint32_t a, b, l;
   // Construct high and low argument for squaring
   a = x & 0xFFFF;
   b = x >> 16;

   // Calculate high and low result of squaring
   h = (((U32V(a * a) >> 17) + U32V(a * b)) >> 15) + b * b;
   l = x * x;

   h ^= l;
   // Return high XOR low
   return h;
   #endif
}

// Calculate the next internal state
void RABBIT_next_state(RABBIT_state *s)
{
   uint32_t g[8], j, t;
   uint32_t x[]={0x4D34D34D, 0xD34D34D3, 0x34D34D34};
   int      i;

   // Calculate new counter values
   for (i=0; i<8; i++) {
       t       = s->c[i];
       s->c[i] = s->c[i] + x[i % 3] + s->carry;
       s->carry = s->c[i] < t;
   }

   // Calculate the g-values
   for (i=0; i<8; i++) {
       g[i] = RABBIT_g_func(s->x[i] + s->c[i]);
   }

   // Calculate new state values
   for (i=0, j=7; i<8;) {
       s->x[i] =t= U32V(g[i] + ROTL32(g[j], 16) + ROTL32(g[j-1], 16));
       i++; j++;
       s->x[i] =t= U32V(g[i] + ROTL32(g[j & 7], 8) + g[j-1]);
       i++; j++;
       j &= 7;
   }
}

// Key setup
void RABBIT_setkey(RABBIT_ctx *c, const void *key)
{
   uint32_t k0, k1, k2, k3, i;
   rabbit_blk *k=(rabbit_blk*)key;

   // Generate four subkeys
   k0 = k->d[0];
   k1 = k->d[1];
   k2 = k->d[2];
   k3 = k->d[3];

   // Generate initial state variables
   c->m.x[0] = k0;
   c->m.x[2] = k1;
   c->m.x[4] = k2;
   c->m.x[6] = k3;

   c->m.x[1] = U32V(k3<<16) | (k2>>16);
   c->m.x[3] = U32V(k0<<16) | (k3>>16);
   c->m.x[5] = U32V(k1<<16) | (k0>>16);
   c->m.x[7] = U32V(k2<<16) | (k1>>16);

   // Generate initial counter values
   c->m.c[0] = ROTL32(k2, 16);
   c->m.c[2] = ROTL32(k3, 16);
   c->m.c[4] = ROTL32(k0, 16);
   c->m.c[6] = ROTL32(k1, 16);

   c->m.c[1] = (k0&0xFFFF0000) | (k1&0xFFFF);
   c->m.c[3] = (k1&0xFFFF0000) | (k2&0xFFFF);
   c->m.c[5] = (k2&0xFFFF0000) | (k3&0xFFFF);
   c->m.c[7] = (k3&0xFFFF0000) | (k0&0xFFFF);

   // Clear carry bit
   c->m.carry = 0;

   // Iterate the system four times
   for (i=0; i<4; i++) {
      RABBIT_next_state(&c->m);
   }

   // Modify the counters
   for (i=0; i<8; i++) {
      c->m.c[i] ^= c->m.x[(i+4) & 7];
   }

   // Copy master instance to work instance
   memcpy (&c->w.x[0], &c->m.x[0], sizeof(RABBIT_state));
}

// IV setup
void RABBIT_setiv(RABBIT_ctx *c, const void* iv)
{
   rabbit_blk *v=(rabbit_blk*)iv;
   uint32_t   sv[4];
   int        i;

   // Generate four subvectors
   sv[0] = v->d[0];
   sv[2] = v->d[1];

   sv[1] = (sv[0]>>16) | (sv[2]&0xFFFF0000);
   sv[3] = (sv[2]<<16) | (sv[0]&0x0000FFFF);

   // Modify counter values
   for (i=0; i<8; i++) {
      c->w.c[i] = c->m.c[i] ^ sv[i & 3];
   }

   // Copy state variables but not carry flag
   memcpy (&c->w.x[0], &c->m.x[0], 32);

   // Iterate the system four times
   for (i=0; i<4; i++) {
      RABBIT_next_state(&c->w);
   }
}

// Encrypt/decrypt a message of any size
void RABBIT_crypt(RABBIT_ctx *c, void* input, uint32_t inlen)
{
   uint32_t   i, len;
   rabbit_blk x;
   uint8_t    *in=(uint8_t*)input;

   for (;;)
   {
     // break on zero length
     if (inlen==0) break;

     // update state
     RABBIT_next_state(&c->w);

     for (i=0; i<4; i++) {
       x.d[i] = c->w.x[i<<1];
     }

     x.d[0] ^= (c->w.x[5]>>16) ^ (c->w.x[3]<<16);
     x.d[1] ^= (c->w.x[7]>>16) ^ (c->w.x[5]<<16);
     x.d[2] ^= (c->w.x[1]>>16) ^ (c->w.x[7]<<16);
     x.d[3] ^= (c->w.x[3]>>16) ^ (c->w.x[1]<<16);

     for (i=0; i<16 && inlen!=0; i++) {
       *in++ ^= x.b[i];
       inlen--;
     }
   }
}
