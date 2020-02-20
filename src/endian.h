#pragma once
#include "byteswap.h"

namespace ai
{
	class endianness
	{
		static constexpr uint32_t test_number = 0x01020304;
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4309 )
#endif
		static constexpr uint8_t first_byte = static_cast<const uint8_t&>(test_number);
#ifdef _MSC_VER
#pragma warning( pop )
#endif
		endianness() = delete;
	public:
		static constexpr bool little = first_byte == 0x04;
		static constexpr bool middle = first_byte == 0x02;
		static constexpr bool big = first_byte == 0x01;
		static_assert(little || big, "Neither big nor small endian!");
	};

	inline p2 order_le(p2 x)
	{
		return endianness::little ? x : byteswap(x);
	}

	inline p4 order_le(p4 x)
	{
		return endianness::little ? x : byteswap(x);
	}

	inline p8 order_le(p8 x)
	{
		return endianness::little ? x : byteswap(x);
	}

	inline p4 order_be(p4 x)
	{
		return endianness::big ? x : byteswap(x);
	}

	inline p8 order_be(p8 x)
	{
		return endianness::big ? x : byteswap(x);
	}

	inline p2 read_le2(const char* data)
	{
		return order_le(*reinterpret_cast<const p2*>(data));
	}

	inline p4 read_le4(const char* data)
	{
		return order_le(*reinterpret_cast<const p4*>(data));
	}

	inline p8 read_le8(const char* data)
	{
		return order_le(*reinterpret_cast<const p8*>(data));
	}
}
