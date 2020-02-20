#pragma once
#include "general.h"
#include "date.h"

namespace ai
{
    static std::string current_time_and_date()
    {
        using namespace date;

        auto now = std::chrono::system_clock::now();
        auto today = date::floor<days>(now);

        std::stringstream ss;
        ss << today << ' ' << date::make_time(now - today) << " UTC";
        return ss.str();
    }
    
    static std::string current_time()
    {
        using namespace date;

        auto now = std::chrono::system_clock::now();
        auto today = date::floor<days>(now);

        std::stringstream ss;
        ss << date::make_time(now - today);
        return ss.str();
    }
    
    template<typename T>
    static T to_integer(const char* b, const char* e)
    {
        T r = 0;
        std::for_each(b, e, [&r](char c)
        {
            if ('0' <= c && c <= '9')
            {
                r *= 10;
                r += c - '0';
            }
        });
        return r;
    }

    template<typename T>
    static T to_integer(const str& s)
    {
        return to_integer<T>(s.data(), s.data() + s.size());
    }

	static bool is_space(char c) { return ' ' == c || '\t' == c || '\n' == c || '\r' == c; }

	static str& normalise_path(str& path)
	{
		std::for_each(path.cbegin(), path.cend(), [&path, i = 0](char ch) mutable
		{
			if ('\\' == ch)
				path[i] = '/';
			++i;
		});
		return path;
	}

    static str unescape_json(const str& json)
    {
        auto r = str();
        r.reserve(json.size());
        
        for (auto b = json.begin(); b != json.end(); )
        {
            auto e = std::find(b, json.end(), '\\');
            std::copy(b, e, std::back_inserter(r));
            if (json.end() == e || json.end() == e + 1)
                return r;
            switch (*(e+1))
            {
            case 'n': r.push_back('\n'); break;
            case '\'': r.push_back('\''); break;
            case '"': r.push_back('"'); break;
            case '&': r.push_back('&'); break;
            case 'r': r.push_back('\r'); break;
            case 't': r.push_back('\t'); break;
            case 'b': r.push_back('\b'); break;
            case 'f': r.push_back('\f'); break;
            case '\\': r.push_back('\\'); break;
            default: break;
            }
            b = e + 2;
        }
        return r;
    }
    
    static str escape_json(const str& json)
    {
            // http://stackoverflow.com/questions/4253367/how-to-escape-a-json-string-containing-newline-characters-using-javascript
        /*    return this.replace(/\\n/g, "\\n")
               .replace(/\\'/g, "\\'")
               .replace(/\\"/g, '\\"')
               .replace(/\\&/g, "\\&")
               .replace(/\\r/g, "\\r")
               .replace(/\\t/g, "\\t")
               .replace(/\\b/g, "\\b")
               .replace(/\\f/g, "\\f");
        */

        auto r = str();
        r.reserve(json.size());
        for (int i = 0; i < json.size(); ++i)
        {
            char ch = json[i];
            switch (ch)
            {
            case '\n': r.push_back('\\'); r.push_back('n'); break;
            case '\'': r.push_back('\\'); r.push_back('\''); break;
            case '"': r.push_back('\\'); r.push_back('"'); break;
            case '&': r.push_back('\\'); r.push_back('&'); break;
            case '\r': r.push_back('\\'); r.push_back('r'); break;
            case '\t': r.push_back('\\'); r.push_back('t'); break;
            case '\b': r.push_back('\\'); r.push_back('b'); break;
            case '\f': r.push_back('\\'); r.push_back('f'); break;
            case '\\': r.push_back('\\'); r.push_back('\\'); break;
            default: r.push_back(ch);
            }
        }
        return r;
    }
    
//    static str directory(str path)
//    {
//        auto slash = std::find(path.crbegin(), path.crend(), '/');
//        if (path.crend() == slash)
//            return "/";
//        return str(path.cbegin(), slash.base() - 1);
//    }

	static str lpad(str s, char ch, int width)
	{
		return s.size() < width ? str(width - s.size(), ch) + s : s;
	}

	static str full_trim(str s)
	{
		auto write_pos = std::accumulate(s.cbegin(), s.cend(), 0, [base = const_cast<char*>(s.data()), space = true](auto write_pos, char c) mutable
		{
			if (space)
			{
				if (is_space(c))
					return write_pos;
				*(base + write_pos) = c;
				space = false;
			}
			else if (is_space(c))
			{
				*(base + write_pos) = ' ';
				space = true;
			}
			else
				*(base + write_pos) = c;

			return write_pos + 1;
		});
		if (write_pos > 0 && ' ' == s[write_pos-1])
			--write_pos;
		return s.substr(0, write_pos);
	}

	static arr<str> split(const str& text, char separator)
	{
		if (text.empty())
			return {};
		auto r = arr<str>();
		auto last_separator = std::accumulate(text.begin(), text.end(), 0, [i = 0, text, &r, separator](int acc, char c) mutable
		{
			if (separator == c)
			{
				r.push_back(str(text.begin() + acc, text.begin() + i));
				acc = i + 1;
			}
			++i;
			return acc;
		});
		r.push_back(str(text.begin() + last_separator, text.end()));
		return r;
	}

	static str json_unescape(const char* b, const char* e)
	{
		auto r = str();
		r.reserve(e - b);
		std::for_each(b, e, [&r, slash = false, digits = 0, digit = 0](char c) mutable
		{
			if (slash)
			{
				slash = false;
				if ('u' == c)
					digits = 1;
				else if ('b' == c)
					r.push_back('\b');
				else if ('f' == c)
					r.push_back('\f');
				else if ('n' == c)
					r.push_back('\n');
				else if ('r' == c)
					r.push_back('\r');
				else if ('t' == c)
					r.push_back('\t');
				else
					r.push_back(c);
				return;
			}

			if ('\\' == c)
			{
				slash = true;
				return;
			}

			if (digits)
			{
				switch (digits)
				{
				case 1: digits = 2; digit = c - '0'; break;
				case 2: digits = 3; r.push_back((digit << 4) | static_cast<int>(c - '0')); break;
				case 3: digits = 4; digit = c - '0'; break;
				default:
					digits = 0;
					r.push_back((digit << 4) | static_cast<int>(c - '0'));
				}
				return;
			}
			r.push_back(c);
		});
		return r;
	}
}
