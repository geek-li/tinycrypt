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
  
#ifndef MD2_H
#define MD2_H

#include <stdint.h>

#define MD2_CBLOCK        16
#define MD2_DIGEST_LENGTH 16
#define MD2_STATE_LEN     48

typedef struct _MD2_CTX { 
  union {
    uint8_t v8[MD2_CBLOCK];
    uint32_t v32[MD2_CBLOCK/4];
  } data;
  
  union {
    uint8_t v8[MD2_STATE_LEN];
    uint32_t v32[MD2_STATE_LEN/4];
  } state;
  
  union {
    uint8_t v8[MD2_DIGEST_LENGTH];
    uint32_t v32[MD2_DIGEST_LENGTH/4];
  } checksum;
  
  uint32_t len;
} MD2_CTX; 

#ifdef __cplusplus
extern "C" {
#endif

  void MD2_Init (MD2_CTX*);
  void MD2_Update (MD2_CTX*, void*, uint32_t);
  void MD2_Final (void*, MD2_CTX*);

#ifdef __cplusplus
}
#endif

#endif
