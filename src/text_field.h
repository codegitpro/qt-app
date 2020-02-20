#pragma once

#include "text_map.h"

namespace ai
{
	struct text_field
	{
		sz size_on_disk() const
		{
			return sequence_.size_on_disk()
				+ text_map::size_on_disk(names_);
		}
		sz deserialise(p8 version, const char* data)
		{
			sz r = sequence_.deserialise(version, data);
			return r + text_map::deserialise(names_, data + r);
		}
		sz serialise(char* data) const
		{
			sz r = sequence_.serialise(data);
			return r + text_map::serialise(names_, data + r);
		}

		sz push_back(const str& s)
		{
			sz r = names_.size();
			names_.push_back(s);
			sequence_.push_back(static_cast<n4>(r));
			return r;
		}
		sz size() const { return sequence_.size(); }
		int bits() const { return sequence_.bits(); }
		n4 operator[](sz index) const { return sequence_[index]; }
		n4 max_name_length() const
		{
			if (names_.empty())
				return 0;

			auto el = std::max_element(names_.cbegin(), names_.cend(), [](auto&& a, auto&& b)
			{
				return a.size() < b.size();
			});
			return static_cast<n4>(el->size());
		}
		str extract(sz index, n4 width = 0)
		{
			auto& s = names_[sequence_[index]];
			if (!width || s.size() <= width)
				return s;
			return str(s.cbegin(), s.cbegin() + width);
		}
		arr<n4> dump() const
		{
			return arr<n4>(sequence_.cbegin(), sequence_.cend());
		}
	private:
		packed sequence_;
		arr<str> names_;
	};
}