#pragma once
#include "../json.h"
#include "object_detail.h"

using namespace std;

namespace ai
{
	namespace openstack
	{
		struct list_parser
		{
			list_parser(const char* data)
			{
				data_ = data;
				json::parse_value(data, json::parse_space(data, 0), *this);
			}
			void begin_array() {}
			void end_array() {}
			void begin_object() {}

			void end_object()
			{
				if (subdir_.first)
					subdir.push_back(subdir_);
				else
					objects.push_back({ name_, bytes_, hash_, last_modified_, content_type_ });
				hash_ = { 0,0 };
				last_modified_ = { 0,0 };
				bytes_ = { 0,0 };
				name_ = { 0,0 };
				content_type_ = { 0,0 };
				subdir_ = { 0,0 };
			}

			void text(json::type type, p4 begin, p4 end)
			{
				switch (type)
				{
				case json::type::text:
				{
					if (!hash_.first && is_equal("hash", key_))
						hash_ = make_pair(begin, end);
					else if (!last_modified_.first && is_equal("last_modified", key_))
						last_modified_ = make_pair(begin, end);
					else if (!name_.first && is_equal("name", key_))
						name_ = make_pair(begin, end);
					else if (!content_type_.first && is_equal("content_type", key_))
						content_type_ = make_pair(begin, end);
					else if (is_equal("subdir", key_))
						subdir_ = make_pair(begin, end);
					break;
				}
				case json::type::number:
				{
					if (!bytes_.first && is_equal("bytes", key_))
						bytes_ = make_pair(begin, end);
					break;
				}
				default: break;
				}
			}
			void key(p4 begin, p4 end)
			{
				key_ = make_pair(begin, end);
			}
			arr<object_detail> objects;
			arr<par<p4, p4>> subdir;

		private:
			const char* data_;
			par<p4, p4> key_ = { 0,0 };

			par<p4, p4> hash_ = { 0,0 };
			par<p4, p4> last_modified_ = { 0,0 };
			par<p4, p4> bytes_ = { 0,0 };
			par<p4, p4> name_ = { 0,0 };
			par<p4, p4> content_type_ = { 0,0 };
			par<p4, p4> subdir_ = { 0,0 };

			bool is_equal(const char* s, par<p4, p4> piece)
			{
				return !strncmp(data_ + piece.first, s, piece.second - piece.first);
			}
		};
	}
}
