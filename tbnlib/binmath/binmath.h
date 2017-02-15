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

#ifndef BINMATH_H
#define BINMATH_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

  uint64_t dec2bin (char*);
  uint64_t hex2bin (char*);

  char *bin2hex (uint64_t);
  char *bin2dec (uint64_t);

  uint64_t add (uint64_t, uint64_t);
  uint64_t square (uint64_t);
  uint64_t cube (uint64_t);
  uint64_t power (uint64_t, uint64_t);
  uint64_t subtract (uint64_t, uint64_t);
  uint64_t multiply (uint64_t, uint64_t);
  uint64_t modulo (uint64_t, uint64_t);
  void divide (uint64_t*, uint64_t*, uint64_t, uint64_t);

  uint64_t addmod (uint64_t, uint64_t, uint64_t);
  uint64_t submod (uint64_t, uint64_t, uint64_t);
  uint64_t mulmod (uint64_t, uint64_t, uint64_t);
  uint64_t divmod (uint64_t, uint64_t, uint64_t);
  uint64_t powmod (uint64_t, uint64_t, uint64_t);
  uint64_t sqrmod (uint64_t, uint64_t);
  uint64_t invmod (uint64_t, uint64_t);

  uint64_t totient (uint64_t);
  uint64_t nroot (uint64_t, uint64_t);
  uint32_t hash (char*);

  uint64_t gen_prime (uint64_t);
  uint64_t rand_range (uint64_t);
  uint64_t safe_prime (uint64_t);
  uint64_t random (uint64_t);
  void srandom (uint64_t);

  int miller (uint64_t, int);
  int fermat (uint64_t, int);
  uint64_t gcd (uint64_t, uint64_t);

  uint64_t rho (uint64_t);
  uint64_t lcm (uint64_t, uint64_t);
  uint64_t getbits (uint64_t);

#ifdef __cplusplus
}
#endif

#endif