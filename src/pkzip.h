#pragma once
#include <zlib.h>
#include "endian.h"

namespace ai
{

	enum class zlib_header
	{
		deflate,
		gzip,
		zlib,
		autodetect
	};

	char* zlib_decompress(const char* compressed_data, p4 compressed_size, p4 uncompressed_size, zlib_header header = zlib_header::deflate)
	{
		auto zstr = z_stream();
		zstr.zalloc = Z_NULL;
		zstr.zfree = Z_NULL;
		zstr.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data));
		zstr.avail_in = compressed_size;
		char* result = reinterpret_cast<char*>(malloc(uncompressed_size + 1));
		zstr.next_out = reinterpret_cast<Bytef*>(result);
		zstr.avail_out = uncompressed_size;
		// https://stackoverflow.com/questions/1838699/how-can-i-decompress-a-gzip-stream-with-zlib
		int bits = MAX_WBITS;
		switch (header)
		{
		case zlib_header::deflate: bits = -bits; break;
		case zlib_header::gzip: bits |= 16; break;
		case zlib_header::autodetect: bits |= 32; break;
		}

		auto res = inflateInit2(&zstr, bits);
		int r = inflate(&zstr, Z_FINISH);
		if (Z_STREAM_END == r)
		{
			*(result + uncompressed_size) = '\0';
			return result;
		}
		free(result);
		return nullptr;
	}

	struct pkzip
	{

		enum class method : p2
		{
			stored = 0,
			shrunk,
			reduced1,
			reduced2,
			reduced3,
			reduced4,
			imploded,
			tokenizing,
			deflated,
			deflate64,
			pk_imploding,
			pk_reserved,
			bzip2
		};

		pkzip(const char** data)
		{
			*data += 4;
			version_needed_to_extract = read_le2(*data);
			*data += 2;
			general_purpose_bit_flag = read_le2(*data);
			*data += 2;
			compression_method = static_cast<method>(read_le2(*data));
			*data += 2;
			last_mod_file_time = read_le2(*data);
			*data += 2;
			last_mod_file_date = read_le2(*data);
			*data += 2;
			crc_32 = read_le4(*data);
			*data += 4;
			compressed_size = read_le4(*data);
			*data += 4;
			uncompressed_size = read_le4(*data);
			*data += 4;
			file_name_length = read_le2(*data);
			*data += 2;
			extra_field_length = read_le2(*data);
			*data += 2;
			file_name = *data;
			extra_field = *data + file_name_length;
			*data += file_name_length + extra_field_length;
			compressed_data = *data;
			*data += compressed_size;

			if ((general_purpose_bit_flag & 0x8) == 0x8)
				*data += 12;
		}
		//	local file header signature     4 bytes(0x04034b50)
		p2 version_needed_to_extract;
		p2 general_purpose_bit_flag;
		method compression_method;
		p2 last_mod_file_time;
		p2 last_mod_file_date;
		p4 crc_32;
		p4 compressed_size;
		p4 uncompressed_size;
		p2 file_name_length;
		p2 extra_field_length;
		const char*	file_name;
		const char* extra_field;
		const char* compressed_data;

		char* decompress()
		{
			return zlib_decompress(compressed_data, compressed_size, uncompressed_size, zlib_header::deflate);
		}
	};

}