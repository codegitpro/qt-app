#pragma once
#include "pack.h"

namespace ai
{
	struct text_map
	{
		arr<str>::const_iterator cbegin() const { return names_.cbegin(); }
		arr<str>::const_iterator cend() const { return names_.cend(); }

		sz size_on_disk() const { return size_on_disk(names_); }
		sz deserialise(const p1* data) { return deserialise_arr_str(data, names_); }
		sz serialise(p1* data) const { return serialise_arr_str(data, names_); }

		void push_back(const str& name) { names_.push_back(name); }
		const str& operator[](sz pos) const { return names_[pos]; }
		sz size() const { return names_.size(); }
		void shrink() { name_map_.clear(); }

		arr<str>::const_iterator find(const str& search)
		{
			build_map();
			auto it = name_map_.find(search);
			if (name_map_.end() == it)
				return names_.cend();
			return names_.cbegin() + it->second;
		}

		n4 find_position(const str& search)
		{
			build_map();
			auto it = name_map_.find(search);
			return name_map_.end() != it ? it->second : -1;
		}

		n4 add_if_not_exists(const str& search)
		{
			build_map();
			auto it = name_map_.find(search);
			if (name_map_.end() != it)
				return it->second;
			n4 r = static_cast<n4>(names_.size());
			name_map_[search] = r;
			names_.push_back(search);
			return r;
		}

		n4 add_without_check(const str& search)
		{
			n4 r = static_cast<n4>(names_.size());
			names_.push_back(search);
			return r;
		}

		static sz size_on_disk(const arr<str>& names)
		{
			return sizeof(sz) + std::accumulate(names.cbegin(), names.cend(), static_cast<sz>(0), [](sz tot, auto&& s)
			{
				return tot + sizeof(sz) + s.size();
			});
		}

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
	private:
		hmap<str, n4> name_map_;
		arr<str> names_;

		void build_map()
		{
			if (name_map_.size() == names_.size())
				return;
			assert(name_map_.size() < names_.size());
			std::for_each(names_.cbegin() + name_map_.size(), names_.cend(), [this, i = static_cast<n4>(name_map_.size())](auto&& name) mutable
			{
				name_map_[name] = i++;
			});
			assert(name_map_.size() == names_.size());
		}
	};

}