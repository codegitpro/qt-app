#pragma once
#ifdef _MSC_VER
#define NOMINMAX

#pragma warning( push )
#pragma warning( disable: 4530 )
#endif

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <random>
#include <unordered_map>
#include <limits>
#include <cstddef>
#include <mutex>
#include <sstream>
#include <array>

#ifdef _MSC_VER
#pragma warning( pop )
#endif

namespace ai
{

	using gate = std::unique_lock<std::mutex>;
	using lock = std::lock_guard<std::recursive_mutex>;

	using n1 = int8_t;
	using n2 = int16_t;
	using n4 = int32_t;
	using n8 = int64_t;

	using p1 = uint8_t;
	using p2 = uint16_t;
	using p4 = uint32_t;
	using p8 = uint64_t;
	using p16 = std::pair<uint64_t, uint64_t>;
	constexpr p4 p4max = std::numeric_limits<p4>::max();
	constexpr p8 p8max = std::numeric_limits<p8>::max();
	constexpr n4 n4max = std::numeric_limits<n4>::max();
	constexpr n8 n8max = std::numeric_limits<n8>::max();

	using sz = size_t;

	using r4 = float;
	using r8 = double;

	constexpr n8 KB = 1024;
	constexpr n8 MB = KB * KB;
	constexpr n8 GB = KB * MB;
	constexpr n8 TB = KB * GB;
	constexpr n8 PB = KB * TB;

	template<typename T>
	using uptr = std::unique_ptr<T>;

	template<typename T>
	using sptr = std::shared_ptr<T>;

	template<typename T>
	using wptr = std::weak_ptr<T>;

	template<typename T>
	using funk = std::function<T>;

	template<typename T>
	using arr = std::vector<T>;

	template<typename T, typename U>
	using par = std::pair<T, U>;

	using str = std::string;

	using void_f = funk<void()>;
	using bool_f = funk<bool()>;

	template<typename T, typename R>
	using hmap = std::unordered_map<T, R>;


	struct io
    {
        virtual ~io() {}
		enum class reason
		{
			eof,
			error
		};
		virtual void io_close(reason) = 0;
	};
	struct io_receiver : io
	{
		virtual void io_receive(str) = 0;
	};
	struct io_sender : io
	{
		virtual void io_ready_for_send() = 0;
		virtual void connect_writer(io_receiver&) = 0;
	};

    static inline str to_str(io::reason r)
    {
        switch (r)
        {
        case io::reason::eof: return "eof";
        case io::reason::error: return "error";
        }
        return "unsupported";
    }

    enum class fault_type
	{
		none,
		file,
		http,
		openstack
	};
	struct fault
	{
		static uptr<fault> file(int code) { return std::make_unique<fault>(fault_type::file, code, "", ""); }
		static uptr<fault> http(int code, const str& error) { return std::make_unique<fault>(fault_type::http, code, error, ""); }
		static uptr<fault> openstack(int code, const str& error) { return std::make_unique<fault>(fault_type::openstack, code, error, ""); }

		fault_type family = fault_type::none;
		int code = 0;
		str name;
		str detail;
		fault() {}
		fault(fault_type family, int code, const str& name, const str& detail) : family(family), code(code), name(name), detail(detail) {}
		str to_string() { return std::to_string(code) + " " + name + ": " + detail; }
	};

    static inline char hex_char(p1 x) { return x <= 9 ? '0' + x : 'a' + (x - 10); }

    template<typename T>
    static str to_hex(T x)
    {
        auto r = str(sizeof(T) * 2, ' ');
        for (sz i = 0; i < sizeof(T); ++i)
        {
            r[sizeof(T) * 2 - 2 * i - 2] = hex_char((p1)((x >> 4) & 0xf));
            r[sizeof(T) * 2 - 2 * i - 1] = hex_char((p1)(x & 0xf));
            x >>= 8;
        }
        return r;
    }

    static str to_hex(p16 x) { return to_hex(x.second) + to_hex(x.first); }

    static inline p1 from_hex_char(char c) { return '0' <= c && c <= '9' ? c - '0' : c - 'a' + 10; }

    template<typename T>
    static T from_hex(const char* s)
    {
        T r = 0;
        for (sz i = 0; i < sizeof(T); ++i)
        {
            T z = from_hex_char(*s++);
            z <<= 4;
            z |= from_hex_char(*s++);
            r |= z << ((sizeof(T) - 1 - i) * 8);
        }
        return r;
    }

    static p16 from_hex_16(const char* s) { return std::make_pair(from_hex<p8>(s + 16), from_hex<p8>(s)); }

    static str strip(const char* b, const char* e)
    {
        auto is_space = [](char c) { return ' ' == c || '\t' == c || '\n' == c || '\r' == c; };
        b = std::find_if_not(b, e, is_space);
        if (b == e)
            return "";
        auto it = e - 1;
        for (; it != b; --it)
        {
            if (!is_space(*it))
                break;
        }
        ++it;
        return str(b, it);
    }

    static str replace(const str& original, const str& search, const str& replace)
    {
        auto e = std::search(original.cbegin(), original.cend(), search.cbegin(), search.cend());
        if (original.cend() == e)
            return original;
        auto r = str();
        r.reserve(original.size());
        auto b = original.cbegin();
        while (original.cend() != e)
        {
            std::copy(b, e, std::back_inserter(r));
            r += replace;
            b = e + search.size();
            e = std::search(b, original.cend(), search.cbegin(), search.cend());
        }
        std::copy(b, e, std::back_inserter(r));
        return r;
    }

    template<typename T>
	sz size_on_disk_number()
	{
		return sizeof(T);
	}

	template<typename T>
	sz size_on_disk_number(T&)
	{
		return sizeof(T);
	}

	template<typename T>
	sz deserialise_number(const p1* data, T& value)
	{
		auto len = sizeof(T);
		std::copy(data, data + len, reinterpret_cast<p1*>(&value));
		return len;
	}

	template<typename T>
	sz serialise_number(p1* data, const T& value)
	{
		auto len = sizeof(T);
		auto b = reinterpret_cast<const p1*>(&value);
		std::copy(b, b + len, data);
		return len;
	}

	template<typename T>
	sz size_on_disk_arr(const arr<T>& values)
	{
		return size_on_disk_number<sz>() + sizeof(T)*values.size();
	}

	template<typename T>
	sz deserialise_arr(const p1* data, arr<T>& values)
	{
		values.clear();
		sz size;
		auto r = deserialise_number<sz>(data, size);
		values.resize(size);
		if (size)
		{
			auto len = sizeof(T) * size;
			std::copy(data + r, data + r + len, reinterpret_cast<p1*>(values.data()));
			r += len;
		}
		return r;
	}

	template<typename T>
	sz serialise_arr(p1* data, const arr<T>& values)
	{
		auto r = serialise_number<sz>(data, values.size());
		
		auto len = sizeof(T) * values.size();
		auto b = reinterpret_cast<const p1*>(values.data());
		std::copy(b, b + len, data + r);
		return r + len;
	}

	template<typename T>
	sz size_on_disk_arr_arr(const arr<arr<T>>& arrays)
	{
        sz start = 0;
		return size_on_disk_number<sz>() 
			+ std::accumulate(arrays.cbegin(), arrays.cend(), start, [](sz acc, const arr<T>& values) { return acc + size_on_disk_arr(values); });
	}

	template<typename T>
	sz deserialise_arr_arr(const p1* data, arr<arr<T>>& arrays)
	{
		arrays.clear();
		sz size;
		auto r = deserialise_number<sz>(data, size);
		for (sz i = 0; i < size; ++i)
		{
			arr<T> values;
			r += deserialise_arr<T>(data + r, values);
			arrays.push_back(std::move(values));
		}
		return r;
	}

	template<typename T>
	sz serialise_arr_arr(p1* data, const arr<arr<T>>& arrays)
	{
		auto r = serialise_number<sz>(data, arrays.size());

		r += std::accumulate(arrays.cbegin(), arrays.cend(), 0ull, [data = data + r](sz acc, const arr<T>& values)
		{
			acc += serialise_arr<T>(data + acc, values);
			return acc;
		});

		return r;
	}

	template<typename T, typename U>
	sz size_on_disk_hmap(const hmap<T,U>& values)
	{
		return size_on_disk_number<sz>() + sizeof(par<T,U>)*values.size();
	}

	template<typename T, typename U>
	sz deserialise_hmap(const p1* data, hmap<T, U>& values)
	{
		values.clear();
		sz size;
		auto r = deserialise_number<sz>(data, size);
		auto p = reinterpret_cast<const par<T, U>*>(data + r);
		std::for_each(p, p + size, [&r, &values](const par<T, U>& item)
		{
			values[item.first] = item.second;
			r += sizeof(par<T, U>);
		});
		return r;
	}

	template<typename T, typename U>
	sz serialise_hmap(p1* data, const hmap<T,U>& values)
	{
		auto r = serialise_number<sz>(data, values.size());
		std::for_each(values.cbegin(), values.cend(), [&r, data](auto&& item)
		{
			auto p = reinterpret_cast<par<T, U>*>(data + r);
			*p = item;
			r += sizeof(par<T, U>);
		});
		return r;
	}

	static sz size_on_disk_str(const str& s)
	{
		return size_on_disk_number<sz>() + s.size();
	}

	static sz deserialise_str(const p1* data, str& s)
	{
		sz size;
		auto r = deserialise_number<sz>(data, size);
		s.resize(size);
		if (size)
		{
			std::copy(data + r, data + r + size, const_cast<char*>(s.data()));
			r += size;
		}
		return r;
	}

	static sz serialise_str(p1* data, const str& s)
	{
		auto r = serialise_number<sz>(data, s.size());

		auto len = s.size();
		auto b = s.data();
		std::copy(b, b + len, data + r);
		return r + len;
	}

	static sz size_on_disk_arr_str(const arr<str>& arrays)
	{
        sz start = 0;
		return size_on_disk_number<sz>()
			+ std::accumulate(arrays.cbegin(), arrays.cend(), start, [](sz acc, const str& values) { return acc + size_on_disk_str(values); });
	}

	static sz deserialise_arr_str(const p1* data, arr<str>& arrays)
	{
		arrays.clear();
		sz size;
		auto r = deserialise_number<sz>(data, size);
		for (sz i = 0; i < size; ++i)
		{
			str values;
			r += deserialise_str(data + r, values);
			arrays.push_back(std::move(values));
		}
		return r;
	}

	static sz serialise_arr_str(p1* data, const arr<str>& arrays)
	{
		auto r = serialise_number<sz>(data, arrays.size());

		r += std::accumulate(arrays.cbegin(), arrays.cend(), 0ull, [data = data + r](sz acc, const str& values)
		{
			acc += serialise_str(data + acc, values);
			return acc;
		});

		return r;
	}

	static sz common_file_path_length(const str& path1, const str& path2)
	{
		auto common_ends = mismatch(path1.cbegin(), path1.cend(), path2.cbegin(), path2.cend());
		auto slash_it = std::find(std::make_reverse_iterator(common_ends.first), path1.crend(), '/').base();
		return std::distance(path1.cbegin(), slash_it);
	}
 
    struct ref_count
    {
        ref_count(int& count) : count_(count)
        {
            ++count_;
        }
        ~ref_count()
        {
            --count_;
        }
        int count() const { return count_; }
        bool multi() const { return count_ > 1; }
    private:
        int& count_;
    };
}
