#pragma once
#include "general.h"

#if defined(_MSC_VER)
//  Microsoft documents these as being compatible since Windows 95 and specifically
//  lists runtime library support since Visual Studio 2003 (aka 7.1).
//  Clang/c2 uses the Microsoft rather than GCC intrinsics, so we check for
//  defined(_MSC_VER) before defined(__clang__)
# define BOOST_ENDIAN_INTRINSIC_MSG "cstdlib _byteswap_ushort, etc."
# include <cstdlib>
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) _byteswap_ushort(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_4(x) _byteswap_ulong(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_8(x) _byteswap_uint64(x)

//  GCC and Clang recent versions provide intrinsic byte swaps via builtins
#elif (defined(__GNUC__ ) && \
  (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
# define BOOST_ENDIAN_INTRINSIC_MSG "__builtin_bswap16, etc."
// prior to 4.8, gcc did not provide __builtin_bswap16 on some platforms so we emulate it
// see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
// Clang has a similar problem, but their feature test macros make it easier to detect
# if (defined(__GNUC__) &&(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
#   define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap16(x)
# else
#   define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap32((x) << 16)
# endif
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_4(x) __builtin_bswap32(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_8(x) __builtin_bswap64(x)

#elif (defined(__clang__) && __has_builtin(__builtin_bswap32) && __has_builtin(__builtin_bswap64)) \
  || (defined(__GNUC__ ) && \
  (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
# define BOOST_ENDIAN_INTRINSIC_MSG "__builtin_bswap16, etc."
// prior to 4.8, gcc did not provide __builtin_bswap16 on some platforms so we emulate it
// see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
// Clang has a similar problem, but their feature test macros make it easier to detect
# if (defined(__clang__) && __has_builtin(__builtin_bswap16)) \
  || (defined(__GNUC__) &&(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
#   define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap16(x)
# else
#   define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap32((x) << 16)
# endif
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_4(x) __builtin_bswap32(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_8(x) __builtin_bswap64(x)

//  Linux systems provide the byteswap.h header, with
#elif defined(__linux__)
//  don't check for obsolete forms defined(linux) and defined(__linux) on the theory that
//  compilers that predefine only these are so old that byteswap.h probably isn't present.
# define BOOST_ENDIAN_INTRINSIC_MSG "byteswap.h bswap_16, etc."
# include <byteswap.h>
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x) bswap_16(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_4(x) bswap_32(x)
# define BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_8(x) bswap_64(x)

#else
# define BOOST_ENDIAN_NO_INTRINSICS
# define BOOST_ENDIAN_INTRINSIC_MSG "no byte swap intrinsics"
#endif

namespace ai
{


	inline p2 byteswap(p2 x)
	{
# ifdef BOOST_ENDIAN_NO_INTRINSICS  
		return (x << 8) | (x >> 8);
# else
		return BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_2(x);
# endif
	}

	inline p4 byteswap(p4 x)
	{
# ifdef BOOST_ENDIAN_NO_INTRINSICS  
		p4 step16;
		step16 = x << 16 | x >> 16;
		return
			((step16 << 8) & 0xff00ff00)
			| ((step16 >> 8) & 0x00ff00ff);
# else
		return BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_4(x);
# endif
	}

	inline p8 byteswap(p8 x)
	{
# ifdef BOOST_ENDIAN_NO_INTRINSICS  
		p8 step32, step16;
		step32 = x << 32 | x >> 32;
		step16 = (step32 & 0x0000FFFF0000FFFFULL) << 16
			| (step32 & 0xFFFF0000FFFF0000ULL) >> 16;
		return (step16 & 0x00FF00FF00FF00FFULL) << 8
			| (step16 & 0xFF00FF00FF00FF00ULL) >> 8;
# else
		return BOOST_ENDIAN_INTRINSIC_BYTE_SWAP_8(x);
# endif
	}

	inline n2 byteswap(n2 x)
	{
		return static_cast<n2>(byteswap(static_cast<p2>(x)));
	}

	inline n4 byteswap(n4 x)
	{
		return static_cast<n4>(byteswap(static_cast<p4>(x)));
	}

	inline n8 byteswap(n8 x)
	{
		return static_cast<n8>(byteswap(static_cast<p8>(x)));
	}

	inline n1 byteswap(n1 x) { return x; }
	inline p1 byteswap(p1 x) { return x; }
	inline bool byteswap(bool x)
	{
		static_assert(sizeof(bool) == 1, "bool size must be 1");
		return x;
	}
}
