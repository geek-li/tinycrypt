/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#include "cpuinfo.h"
#include "portability.h"
#include "sm3.h"
#include <memory.h>
//#define CPPCRYPTO_DEBUG

#ifdef _MSC_VER
#define inline __forceinline
#endif

namespace cppcrypto
{
	sm3::~sm3()
	{
		clear();
	}

	sm3::sm3()
	{
	}

	static inline uint32_t xorf(uint32_t x, uint32_t y, uint32_t z)
	{
		return x ^ y ^ z;
	}

	static inline uint32_t ff1(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & y) ^ (x & z) ^ (y & z);
	}

	static inline uint32_t gg1(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & y) ^ (~x & z);
	}

	static inline uint32_t p0(uint32_t x)
	{
		return x ^ rotatel32(x, 9) ^ rotatel32(x, 17);
	}

	static inline uint32_t p1(uint32_t x)
	{
		return x ^ rotatel32(x, 15) ^ rotatel32(x, 23);
	}


	void sm3::update(const uint8_t* data, size_t len)
	{
		if (pos && pos + len >= 64)
		{
			memcpy(&m[0] + pos, data, 64 - pos);
			transform(&m[0], 1);
			len -= 64 - pos;
			total += (64 - pos) * 8;
			data += 64 - pos;
			pos = 0;
		}
		if (len >= 64)
		{
			size_t blocks = len / 64;
			size_t bytes = blocks * 64;
			transform(data, blocks);
			len -= bytes;
			total += (bytes)* 8;
			data += bytes;
		}
		memcpy(&m[0] + pos, data, len);
		pos += len;
		total += len * 8;
	}

	void sm3::init()
	{
		H[0] = 0x7380166f;
		H[1] = 0x4914b2b9;
		H[2] = 0x172442d7;
		H[3] = 0xda8a0600;
		H[4] = 0xa96f30bc;
		H[5] = 0x163138aa;
		H[6] = 0xe38dee4d;
		H[7] = 0xb0fb0e4e;
		pos = 0;
		total = 0;
	};

	void sm3::transform(const uint8_t* mp, uint64_t num_blks)
	{
		for (uint64_t blk = 0; blk < num_blks; blk++)
		{
			uint32_t M[16];
			for (uint32_t i = 0; i < 64 / 4; i++)
			{
				M[i] = swap_uint32((reinterpret_cast<const uint32_t*>(mp)[blk * 16 + i]));
			}
#ifdef	CPPCRYPTO_DEBUG
			printf("M1 - M8: %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
				M[0], M[1], M[2], M[3], M[4], M[5], M[6], M[7], M[8], M[9], M[10], M[11], M[12], M[13], M[14], M[15]);
#endif

			uint32_t W[68];
			uint32_t W2[64];
			for (int t = 0; t <= 15; t++)
				W[t] = M[t];
			for (int t = 16; t <= 67; t++)
				W[t] = p1(W[t - 16] ^ W[t - 9] ^ rotatel32(W[t - 3], 15)) ^ rotatel32(W[t - 13], 7) ^ W[t - 6];
			for (int t = 0; t <= 63; t++)
				W2[t] = W[t] ^ W[t + 4];

			uint32_t a = H[0];
			uint32_t b = H[1];
			uint32_t c = H[2];
			uint32_t d = H[3];
			uint32_t e = H[4];
			uint32_t f = H[5];
			uint32_t g = H[6];
			uint32_t h = H[7];

#ifdef	CPPCRYPTO_DEBUG
			printf("===============================================\n");
			printf("i = %d: %08X %08X %08X %08X %08X %08X %08X %08X\n",
				-1, a, b, c, d, e, f, g, h);

			for (int i = 0; i <= 67; i++)
				printf("W[%d] = %08X\n", i, W[i]);
			for (int i = 0; i <= 63; i++)
				printf("W2[%d] = %08X\n", i, W2[i]);
#endif

			for (int t = 0; t <= 15; t++)
			{
				uint32_t ss1 = rotatel32((rotatel32(a, 12) + e + rotatel32(0x79cc4519, t)), 7);
				uint32_t ss2 = ss1 ^ rotatel32(a, 12);
				uint32_t tt1 = xorf(a, b, c) + d + ss2 + W2[t];
				uint32_t tt2 = xorf(e, f, g) + h + ss1 + W[t];
				d = c;
				c = rotatel32(b, 9);
				b = a;
				a = tt1;
				h = g;
				g = rotatel32(f, 19);
				f = e;
				e = p0(tt2);
#ifdef	CPPCRYPTO_DEBUG
				printf("t = %d: %08X %08X %08X %08X %08X %08X %08X %08X (ss1=%08X ss2=%08X tt1=%08X tt2=%08X)\n",
					t, a, b, c, d, e, f, g, h, ss1, ss2, tt1, tt2);
#endif

			}
			for (int t = 16; t <= 63; t++)
			{
				uint32_t ss1 = rotatel32((rotatel32(a, 12) + e + rotatel32(0x7a879d8a, t)), 7);
				uint32_t ss2 = ss1 ^ rotatel32(a, 12);
				uint32_t tt1 = ff1(a, b, c) + d + ss2 + W2[t];
				uint32_t tt2 = gg1(e, f, g) + h + ss1 + W[t];
				d = c;
				c = rotatel32(b, 9);
				b = a;
				a = tt1;
				h = g;
				g = rotatel32(f, 19);
				f = e;
				e = p0(tt2);
#ifdef	CPPCRYPTO_DEBUG
				printf("t = %d: %08X %08X %08X %08X %08X %08X %08X %08X (ss1=%08X ss2=%08X tt1=%08X tt2=%08X)\n",
					t, a, b, c, d, e, f, g, h, ss1, ss2, tt1, tt2);
#endif

			}

			H[0] ^= a;
			H[1] ^= b;
			H[2] ^= c;
			H[3] ^= d;
			H[4] ^= e;
			H[5] ^= f;
			H[6] ^= g;
			H[7] ^= h;
#ifdef	CPPCRYPTO_DEBUG
			printf("H[0] - H[7]: %08X %08X %08X %08X %08X %08X %08X %08X\n",
				H[0], H[1], H[2], H[3], H[4], H[5], H[6], H[7]);
#endif
		}
	}

	void sm3::final(uint8_t* hash)
	{
		m[pos++] = 0x80;
		if (pos > 56)
		{
			memset(&m[0] + pos, 0, 64 - pos);
			transform(&m[0], 1);
			pos = 0;
		}
		memset(&m[0] + pos, 0, 56 - pos);
		uint64_t mlen = swap_uint64(total);
		memcpy(&m[0] + (64 - 8), &mlen, 64 / 8);
		transform(&m[0], 1);
		for (int i = 0; i < 8; i++)
		{
			H[i] = swap_uint32(H[i]);
		}
		memcpy(hash, H, 32);
	}

	void sm3::clear()
	{
		zero_memory(H.get(), H.bytes());
		zero_memory(m.data(), m.size() * sizeof(m[0]));
	}

}

