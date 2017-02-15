/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#include "sm4.h"
#include <memory.h>
#include <algorithm>
#include "portability.h"

#ifdef _MSC_VER
#define inline __forceinline
#endif

//#define CPPCRYPTO_DEBUG

namespace cppcrypto
{

	static const uint8_t S[256] = {
		0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
		0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
		0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
		0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
		0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
		0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
		0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
		0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
		0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
		0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
		0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
		0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
		0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
		0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
		0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
		0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
	};

	static inline uint32_t T(uint32_t x)
	{
		x = uint32_t(S[uint8_t(x)])              ^ 
       (uint32_t(S[uint8_t(x >> 8)]) << 8)   ^ 
       (uint32_t(S[uint8_t(x >> 16)]) << 16) ^ 
       (uint32_t(S[uint8_t(x >> 24)]) << 24);
       
		return x ^ rotatel32(x, 2) ^ rotatel32(x, 10) ^ rotatel32(x, 18) ^ rotatel32(x, 24);
	}

	static inline uint32_t TK(uint32_t x)
	{
		x = uint32_t(S[uint8_t(x)])              ^ 
       (uint32_t(S[uint8_t(x >> 8)]) << 8)   ^ 
       (uint32_t(S[uint8_t(x >> 16)]) << 16) ^ 
       (uint32_t(S[uint8_t(x >> 24)]) << 24);
       
		return x ^ rotatel32(x, 13) ^ rotatel32(x, 23);
	}

	static inline uint32_t F(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3, uint32_t rk)
	{
		return x0 ^ T(x1 ^ x2 ^ x3 ^ rk);
	}

	sm4::~sm4()
	{
		clear();
	}

	void sm4::clear()
	{
		zero_memory(rk, sizeof(rk));
	}

	bool sm4::init(const uint8_t* key, block_cipher::direction direction)
	{
		uint32_t k0 = swap_uint32(*(uint32_t*)key) ^ 0xa3b1bac6;
		uint32_t k1 = swap_uint32(*(((uint32_t*)key) + 1)) ^ 0x56aa3350;
		uint32_t k2 = swap_uint32(*(((uint32_t*)key) + 2)) ^ 0x677d9197;
		uint32_t k3 = swap_uint32(*(((uint32_t*)key) + 3)) ^ 0xb27022dc;

		rk[0] = k0 ^ TK(k1 ^ k2 ^ k3 ^ 0x00070e15);
		rk[1] = k1 ^ TK(k2 ^ k3 ^ rk[0] ^ 0x1c232a31);
		rk[2] = k2 ^ TK(k3 ^ rk[0] ^ rk[1] ^ 0x383f464d);
		rk[3] = k3 ^ TK(rk[0] ^ rk[1] ^ rk[2] ^ 0x545b6269);
		rk[4] = rk[0] ^ TK(rk[1] ^ rk[2] ^ rk[3] ^ 0x70777e85);
		rk[5] = rk[1] ^ TK(rk[2] ^ rk[3] ^ rk[4] ^ 0x8c939aa1);
		rk[6] = rk[2] ^ TK(rk[3] ^ rk[4] ^ rk[5] ^ 0xa8afb6bd);
		rk[7] = rk[3] ^ TK(rk[4] ^ rk[5] ^ rk[6] ^ 0xc4cbd2d9);

		rk[8] = rk[4] ^ TK(rk[5] ^ rk[6] ^ rk[7] ^ 0xe0e7eef5);
		rk[9] = rk[5] ^ TK(rk[6] ^ rk[7] ^ rk[8] ^ 0xfc030a11);
		rk[10] = rk[6] ^ TK(rk[7] ^ rk[8] ^ rk[9] ^ 0x181f262d);
		rk[11] = rk[7] ^ TK(rk[8] ^ rk[9] ^ rk[10] ^ 0x343b4249);
		rk[12] = rk[8] ^ TK(rk[9] ^ rk[10] ^ rk[11] ^ 0x50575e65);
		rk[13] = rk[9] ^ TK(rk[10] ^ rk[11] ^ rk[12] ^ 0x6c737a81);
		rk[14] = rk[10] ^ TK(rk[11] ^ rk[12] ^ rk[13] ^ 0x888f969d);
		rk[15] = rk[11] ^ TK(rk[12] ^ rk[13] ^ rk[14] ^ 0xa4abb2b9);

		rk[16] = rk[12] ^ TK(rk[13] ^ rk[14] ^ rk[15] ^ 0xc0c7ced5);
		rk[17] = rk[13] ^ TK(rk[14] ^ rk[15] ^ rk[16] ^ 0xdce3eaf1);
		rk[18] = rk[14] ^ TK(rk[15] ^ rk[16] ^ rk[17] ^ 0xf8ff060d);
		rk[19] = rk[15] ^ TK(rk[16] ^ rk[17] ^ rk[18] ^ 0x141b2229);
		rk[20] = rk[16] ^ TK(rk[17] ^ rk[18] ^ rk[19] ^ 0x30373e45);
		rk[21] = rk[17] ^ TK(rk[18] ^ rk[19] ^ rk[20] ^ 0x4c535a61);
		rk[22] = rk[18] ^ TK(rk[19] ^ rk[20] ^ rk[21] ^ 0x686f767d);
		rk[23] = rk[19] ^ TK(rk[20] ^ rk[21] ^ rk[22] ^ 0x848b9299);

		rk[24] = rk[20] ^ TK(rk[21] ^ rk[22] ^ rk[23] ^ 0xa0a7aeb5);
		rk[25] = rk[21] ^ TK(rk[22] ^ rk[23] ^ rk[24] ^ 0xbcc3cad1);
		rk[26] = rk[22] ^ TK(rk[23] ^ rk[24] ^ rk[25] ^ 0xd8dfe6ed);
		rk[27] = rk[23] ^ TK(rk[24] ^ rk[25] ^ rk[26] ^ 0xf4fb0209);
		rk[28] = rk[24] ^ TK(rk[25] ^ rk[26] ^ rk[27] ^ 0x10171e25);
		rk[29] = rk[25] ^ TK(rk[26] ^ rk[27] ^ rk[28] ^ 0x2c333a41);
		rk[30] = rk[26] ^ TK(rk[27] ^ rk[28] ^ rk[29] ^ 0x484f565d);
		rk[31] = rk[27] ^ TK(rk[28] ^ rk[29] ^ rk[30] ^ 0x646b7279);

		if (direction == decryption)
		{
			for (int i = 0; i < 16; i++)
				std::swap(rk[i], rk[31 - i]);
		}
		return true;
	}

	void sm4::encrypt_block(const uint8_t* in, uint8_t* out)
	{
		uint32_t x0 = swap_uint32(*(uint32_t*)in);
		uint32_t x1 = swap_uint32(*(((uint32_t*)in) + 1));
		uint32_t x2 = swap_uint32(*(((uint32_t*)in) + 2));
		uint32_t x3 = swap_uint32(*(((uint32_t*)in) + 3));
		x0 = F(x0, x1, x2, x3, rk[0]);
		x1 = F(x1, x2, x3, x0, rk[1]);
		x2 = F(x2, x3, x0, x1, rk[2]);
		x3 = F(x3, x0, x1, x2, rk[3]);
		x0 = F(x0, x1, x2, x3, rk[4]);
		x1 = F(x1, x2, x3, x0, rk[5]);
		x2 = F(x2, x3, x0, x1, rk[6]);
		x3 = F(x3, x0, x1, x2, rk[7]);
		x0 = F(x0, x1, x2, x3, rk[8]);
		x1 = F(x1, x2, x3, x0, rk[9]);
		x2 = F(x2, x3, x0, x1, rk[10]);
		x3 = F(x3, x0, x1, x2, rk[11]);
		x0 = F(x0, x1, x2, x3, rk[12]);
		x1 = F(x1, x2, x3, x0, rk[13]);
		x2 = F(x2, x3, x0, x1, rk[14]);
		x3 = F(x3, x0, x1, x2, rk[15]);
		x0 = F(x0, x1, x2, x3, rk[16]);
		x1 = F(x1, x2, x3, x0, rk[17]);
		x2 = F(x2, x3, x0, x1, rk[18]);
		x3 = F(x3, x0, x1, x2, rk[19]);
		x0 = F(x0, x1, x2, x3, rk[20]);
		x1 = F(x1, x2, x3, x0, rk[21]);
		x2 = F(x2, x3, x0, x1, rk[22]);
		x3 = F(x3, x0, x1, x2, rk[23]);
		x0 = F(x0, x1, x2, x3, rk[24]);
		x1 = F(x1, x2, x3, x0, rk[25]);
		x2 = F(x2, x3, x0, x1, rk[26]);
		x3 = F(x3, x0, x1, x2, rk[27]);
		x0 = F(x0, x1, x2, x3, rk[28]);
		x1 = F(x1, x2, x3, x0, rk[29]);
		x2 = F(x2, x3, x0, x1, rk[30]);
		x3 = F(x3, x0, x1, x2, rk[31]);

		*(uint32_t*)out = swap_uint32(x3);
		*(((uint32_t*)out) + 1) = swap_uint32(x2);
		*(((uint32_t*)out) + 2) = swap_uint32(x1);
		*(((uint32_t*)out) + 3) = swap_uint32(x0);
	}

	void sm4::decrypt_block(const uint8_t* in, uint8_t* out)
	{
		return encrypt_block(in, out);
	}

}
