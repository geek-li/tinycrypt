/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#ifndef CPPCRYPTO_SM4_H
#define CPPCRYPTO_SM4_H

#include <stdint.h>
#include "block_cipher.h"

namespace cppcrypto
{

	class sm4 : public block_cipher
	{
	public:
		~sm4();

		size_t blocksize() const override { return 128; }
		size_t keysize() const override { return 128; }
		sm4* clone() const override { return new sm4; }
		void clear() override;

		bool init(const uint8_t* key, block_cipher::direction direction) override;
		void encrypt_block(const uint8_t* in, uint8_t* out) override;
		void decrypt_block(const uint8_t* in, uint8_t* out) override;

	private:
		uint32_t rk[32];
	};

}

#endif
