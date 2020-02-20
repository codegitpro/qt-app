#pragma once
#include "general.h"
#include <cstring>

namespace ai
{
	struct json
	{
		enum type
		{
			text,
			number,
			truth,
			falsehood,
			nul
		};

		template<p4 LEN>
		static p4 parse_fixed_value(const char* source, p4 begin_pos, const char* special_value)
		{
			const char* p = source + begin_pos;
			return std::strncmp(p, special_value, LEN) ? 0 : begin_pos + LEN;
		}

		static p4 parse_true(const char* source, p4 pos) { return parse_fixed_value<4>(source, pos, "true"); }
		static p4 parse_false(const char* source, p4 pos) { return parse_fixed_value<5>(source, pos, "false"); }
		static p4 parse_null(const char* source, p4 pos) { return parse_fixed_value<4>(source, pos, "null"); }
		static bool is_digit(char k) { return '0' <= k && k <= '9'; }
		static bool is_solidus(char k) { return '\"' == k || '\\' == k || '/' == k || 'b' == k || 'f' == k || 'n' == k || 'r' == k || 't' == k; }
		static bool is_hex_digit(char k) { return is_digit(k) || ('a' <= k && k <= 'f') || ('A' <= k && k <= 'F'); }

		static p4 parse_space(const char* source, const p4 begin_pos)
		{
			p4 pos = begin_pos;
			char k = *(source + pos);
			while (' ' == k || '\r' == k || '\n' == k || '\t' == k)
				k = *(source + pos++ + 1);
			return begin_pos == pos ? 0 : pos;
		}

		static p1 hex_value(char k)
		{
			if (is_digit(k))
				return k - '0';
			if ('a' <= k && k <= 'f')
				return 10 + k - 'a';
			if ('A' <= k && k <= 'F')
				return 10 + k - 'A';
			return 0;
		}

		static p4 parse_text(const char* source, p4 pos)
		{
			char k = *(source + pos);
			if ('"' != k)
				return 0;
			k = *(source + pos++ + 1);
			while (k)
			{
				if ('\\' == k)
				{
					k = *(source + pos++ + 1);
					if (is_solidus(k))
						k = *(source + pos++ + 1);
					else if ('u' == k)
					{
						p4 pos_4 = pos + 4;
						k = *(source + pos++ + 1);
						while (is_hex_digit(k) && pos < pos_4)
							k = *(source + pos++ + 1);
						if (pos != pos_4)
							return 0;
					}
					else
						return 0;
				}
				else if ('"' == k)
					return pos + 1;
				else if (0 < k && k <= 0x1f)
					return 0;
				else
					k = *(source + pos++ + 1);
			}
			return 0;
		}

		static p4 parse_number(const char* source, p4 begin_pos)
		{
			p4 pos = begin_pos;
			char k = *(source + pos);
			if ('-' == k)
				k = *(source + pos++ + 1);

			if ('0' == k)
				k = *(source + pos++ + 1);
			else if ('1' <= k && k <= '9')
			{
				k = *(source + pos++ + 1);
				while (is_digit(k))
					k = *(source + pos++ + 1);
			}
			else
				return 0;

			if ('.' == k)
			{
				k = *(source + pos++ + 1);
				if (is_digit(k))
				{
					k = *(source + pos++ + 1);
					while (is_digit(k))
						k = *(source + pos++ + 1);
				}
				else
					return 0;
			}

			if ('e' == k || 'E' == k)
			{
				k = *(source + pos++ + 1);
				if ('-' == k || '+' == k)
					k = *(source + pos++ + 1);
				if (is_digit(k))
				{
					k = *(source + pos++ + 1);
					while (is_digit(k))
						k = *(source + pos++ + 1);
					return pos;
				}
				else
					return 0;
			}

			return pos;
		}

		template<typename T>
		static p4 parse_value(const char* source, p4 pos, T& target)
		{
			p4 next = parse_text(source, pos);
			if (next)
			{
				target.text(type::text, pos + 1, next - 1);
				return next;
			}
			next = parse_number(source, pos);
			if (next)
			{
				target.text(type::number, pos, next);
				return next;
			}
			next = parse_true(source, pos);
			if (next)
			{
				target.text(type::truth, pos, next);
				return next;
			}
			next = parse_false(source, pos);
			if (next)
			{
				target.text(type::falsehood, pos, next);
				return next;
			}
			next = parse_null(source, pos);
			if (next)
			{
				target.text(type::nul, pos, next);
				return next;
			}
			next = parse_object(source, pos, target);
			if (next)
				return next;

			next = parse_array(source, pos, target);
			if (next)
				return next;

			return 0;
		}

		template<typename T>
		static p4 parse_property(const char* source, p4 pos, T& target)
		{
			p4 name = parse_text(source, pos);
			if (!name)
				return 0;
			target.key(pos + 1, name - 1);

			p4 space = parse_space(source, name);
			if (space)
				name = space;

			char k = *(source + name++);
			if (':' != k)
				return (std::numeric_limits<p4>::max)();

			space = parse_space(source, name);
			if (space)
				name = space;

			return parse_value(source, name, target);
		}

		template<typename T, typename partial_parser, char left, char right>
		static p4 parse_sequence(const char* source, p4 pos, T& target)
		{
			char k = *(source + pos++);
			if (left != k)
				return 0;
			partial_parser::begin(target);

			while (true)
			{
				p4 space = parse_space(source, pos);
				if (space)
					pos = space;

				p4 prop = partial_parser::parse(source, pos, target);
				if (!prop)
				{
					k = *(source + pos++);
					break;
				}
				if ((std::numeric_limits<p4>::max)() == prop)
					return prop;
				pos = prop;

				space = parse_space(source, pos);
				if (space)
					pos = space;

				k = *(source + pos++);
				if (',' != k)
					break;
			}

			if (right != k)
				return (std::numeric_limits<p4>::max)();

			partial_parser::finish(target);
			return pos;
		}

		template<typename T>
		struct object_parser
		{
			static void begin(T& teiken) { teiken.begin_object(); }
			static void finish(T& teiken) { teiken.end_object(); }
			static p4 parse(const char* bron, p4 pos, T& teiken) { return parse_property(bron, pos, teiken); }
		};

		template<typename T>
		static p4 parse_object(const char* bron, p4 pos, T& teiken)
		{
			return parse_sequence<T, object_parser<T>, '{', '}'>(bron, pos, teiken);
		}

		template<typename T>
		struct array_parser
		{
			static void begin(T& teiken) { teiken.begin_array(); }
			static void finish(T& teiken) { teiken.end_array(); }
			static p4 parse(const char* bron, p4 pos, T& teiken) { return parse_value(bron, pos, teiken); }
		};

		template<typename T>
		static p4 parse_array(const char* bron, p4 pos, T& teiken)
		{
			return parse_sequence<T, array_parser<T>, '[', ']'>(bron, pos, teiken);
		}
	};
}
