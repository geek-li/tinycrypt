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
  
#ifndef TBNLIB_H
#define TBNLIB_H

#include <stdint.h>

#define BIT_LEN 4096  // increase if you want
#define BUF_LEN 8192

typedef uint32_t bn_word;

// big number array
typedef struct _bn_t {
  int neg;
  union {
    uint8_t v8[BUF_LEN/8];
    uint16_t v16[BUF_LEN/16];
    uint32_t v32[BUF_LEN/32];
    uint64_t v64[BUF_LEN/64];
  };
} bn;

typedef union _bw_t {
  uint32_t v32;
  struct {
    uint16_t lo;
    uint16_t hi;
  };
} bw;

typedef union _bd_t {
  uint64_t v64;
  struct {
    uint32_t lo;
    uint32_t hi;
  };
} bd;
  
#ifdef __cplusplus
extern "C" {
#endif

int bn_bt(bn*, uint32_t);
int bn_bits(bn*);
int bn_bytes(bn*);

void bn_zero(bn*);
void bn_copy(bn*, bn*);

void bn_not(bn*);

void bn_add(bn*, bn*, bn*);
void bn_sub(bn*, bn*, bn*);
void bn_mul(bn*, bn*, bn*);
void bn_div(bn*, bn*, bn*, bn*);
void bn_mod(bn*, bn*, bn*);

void bn_add_word(bn*, uint32_t);
void bn_sub_word(bn*, uint32_t);
void bn_mul_word(bn*, uint32_t);
uint32_t bn_div_word(bn*, uint32_t);

int bn_cmp(bn*, bn*);

void bn_addmod(bn*, bn*, bn*, bn*);
void bn_mulmod(bn*, bn*, bn*, bn*);
void bn_expmod(bn*, bn*, bn*, bn*);
void bn_invmod(bn*, bn*, bn*);

void bn_swap(bn*);

char *bn2str(bn*, int);
void str2bn(bn*, int, char*);

#ifdef __cplusplus
}
#endif

#endif