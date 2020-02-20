#pragma once

#include "text_map.h"

namespace ai
{
	struct lookup_field
	{
		n4 add_to_dictionary(const str& s) { return references_.add_if_not_exists(s); }
		sz references_size() const { return references_.size(); }
		sz size() const { return sequence_.size(); }
		int bits() const { return sequence_.bits(); }
		n4 operator[](sz index) const { return sequence_[index]; }
		str reference(sz index) const { return references_[index]; }
		n4 reference_index_for(const str& name) { return references_.find_position(name); }
		n4 max_name_length() const { return references_.max_name_length(); }
		arr<n4> dump() const { return arr<n4>(sequence_.cbegin(), sequence_.cend()); }

		sz size_on_disk() const
		{
			return sequence_.size_on_disk()
				+ references_.size_on_disk();
		}
		sz deserialise(p8 version, const char* data)
		{
			sz r = sequence_.deserialise(version, data);
			return r + references_.deserialise(version, data + r);
		}
		sz serialise(char* data) const
		{
			sz r = sequence_.serialise(data);
			return r + references_.serialise(data + r);
		}

		sz push_back(const str& s)
		{
			auto r = references_.add_if_not_exists(s);
			sequence_.push_back(r);
			return r;
		}

		n4 add_existing_word_id(n4 id)
		{
			auto r = sequence_.size();
			sequence_.push_back(id);
			return r;
		}

		str extract(sz index, n4 width = 0) const
		{
			auto& s = references_[sequence_[index]];
			if (!width || s.size() <= width)
				return s;
			return str(s.cbegin(), s.cbegin() + width);
		}

		arr<par<str, n4>> stats_raw() const
		{
			auto counts = arr<n4>();
			counts.resize(references_.size());
			std::for_each(sequence_.cbegin(), sequence_.cend(), [this, &counts](auto id)
			{
				++counts[id];
			});

			auto r = arr<par<str, n4>>();
			r.reserve(references_.size());
			std::transform(references_.cbegin(), references_.cend(), std::back_inserter(r), [&counts, i = 0](auto&& name) mutable
			{
				return std::make_pair(name, counts[i++]);
			});
			return r;
		}

		arr<par<str, n4>> stats() const
		{
			auto r = stats_raw();
			std::sort(r.begin(), r.end(), [](auto&& l, auto&& r)
			{
				return l.first < r.first;
			});
			return r;
		}

		arr<par<n4, par<str, n4>>> stats_by_count() const
		{
			auto raw = stats_raw();
			auto r = arr<par<n4, par<str, n4>>>();
			r.reserve(raw.size());
			std::transform(raw.cbegin(), raw.cend(), std::back_inserter(r), [i = 0](auto&& p) mutable
			{
				return std::make_pair(i++, p);
			});
			std::sort(r.begin(), r.end(), [](auto&& l, auto&& r)
			{
				return l.second.second > r.second.second;
			});
			return r;
		}

		arr<sz> indexes_from_id(n4 search_id) const
		{
			auto r = arr<sz>();
			sz i = 0;
			std::for_each(sequence_.cbegin(), sequence_.cend(), [&r, &i, search_id](auto&& id)
			{
				if (search_id == id)
					r.push_back(i);
				++i;
			});
			return r;
		}
	private:
		packed sequence_;
		text_map references_;
	};
}