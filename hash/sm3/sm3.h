/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#ifndef CPPCRYPTO_SM3_H
#define CPPCRYPTO_SM3_H

#include "crypto_hash.h"
#include "alignedarray.h"
#include <array>
#include <functional>

namespace cppcrypto
{

	class sm3 : public crypto_hash
	{
	public:
		sm3();
		~sm3();

		void init() override;
		void update(const uint8_t* data, size_t len) override;
		void final(uint8_t* hash) override;

		size_t hashsize() const override { return 256; }
		size_t blocksize() const override { return 512; }
		sm3* clone() const override { return new sm3; }
		void clear() override;

	protected:
		void transform(const uint8_t* m, uint64_t num_blks);

		aligned_pod_array<uint32_t, 8, 32> H;
		std::array<uint8_t, 64> m;
		size_t pos;
		uint64_t total;
	};

}

#endif
