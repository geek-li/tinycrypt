#pragma once

//#include "simonSpeckBasic.h"

#include <stdint.h>
#include <stdlib.h>

// void SimonEncryptBlock64128(u32 PL,u32 PR,u32 &CL, u32 &CR, u32* key,int nn,int keysize,int rounds);
// void SimonEncryptBlockALL(u64 PL,u64 PR,u64 &CL, u64 &CR, u64* key,int nn,int keysize);
// 
// void SimonDecryptBlock64128(u32 CL,u32 CR,u32 &PL, u32 &PR, u32* key,int nn,int keysize,int rounds);

typedef __int64 s64;
typedef unsigned __int64 u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
void SimonEncrypt64(u32 *SR, u32 *SL, u32 *k);
void SimonKeySetup64(const unsigned char* rk, unsigned int* rkexp);
void SimonKeySetup96(unsigned char* rk, uint64_t *rkexp);
//void SimonEncrypt96(u64 *SR, u64 *SL, u64 *k);
void SimonEncrypt128(u64 *SR, u64 *SL, u64 *k);
void SimonKeySetup128(unsigned char* rk, uint64_t *rkexp);
#define ROTL( n, X )    ( ( ( X ) << n ) | ( ( X ) >> ( 32 - n ) ) )
#define ROTL2( n, X, L )    ( ( ( X ) << ( n + 64 - L ) >> (64-L)) | ( ( X ) >> ( L - n ) ) )