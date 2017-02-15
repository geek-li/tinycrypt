#ifndef HASH_H
#define HASH_H

//#include "api.h"
//#include "crypto_hash.h"
//#include "crypto_uint8.h"
//#include "crypto_uint32.h"

#define CRYPTO_BYTES 32
#define CRYPTO_VERSION "1.0a"

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

typedef u8 crypto_uint8;
typedef u32 crypto_uint32;

#define ROUNDS     ((CRYPTO_BYTES<=32?10:14))
#define STATEBYTES ((CRYPTO_BYTES<=32?64:128))
#define STATEWORDS (STATEBYTES/4)
#define STATECOLS  (STATEBYTES/8)

#endif
