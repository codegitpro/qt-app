#pragma once
#include "../general.h"

namespace ai
{
	namespace openstack
	{
		struct object_detail
		{
			par<p4, p4> name;
			par<p4, p4> bytes;
			par<p4, p4> hash;
			par<p4, p4> last_modified;
			par<p4, p4> content_type;

			str get_name(const char* data, sz offset) const 
			{
				return str(data + name.first + offset, data + name.second);
			}
			n8 get_size(const char* data) const { return static_cast<n8>(stoll(str(data + bytes.first, data + bytes.second))); }
			p16 get_hash(const char* data) const
			{
				return hash.second - hash.first == 32 ? from_hex_16(data + hash.first) : std::make_pair<p8, p8>(0, 0);
			}
		};
	}
}
