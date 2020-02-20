#pragma once
#include "../test/libuv/http_request.h"
#include "json.h"
#include "pack.h"
#include "lookup_field.h"
#include "text_field.h"
#include "zz_algorithms.h"

namespace ai
{
	namespace fr24
	{
		struct airport_parser
		{
			struct record
			{
				par<p4, p4> id = { 0,0 };
				par<p4, p4> name = { 0,0 };
				par<p4, p4> iata = { 0,0 };
				par<p4, p4> icao = { 0,0 };
				par<p4, p4> city = { 0,0 };
				par<p4, p4> lat = { 0,0 };
				par<p4, p4> lon = { 0,0 };
				par<p4, p4> country = { 0,0 };
				par<p4, p4> alt = { 0,0 };
				par<p4, p4> size = { 0,0 };
				par<p4, p4> tz_name = { 0,0 };
				par<p4, p4> tz_offset = { 0,0 };
				par<p4, p4> tz_offsetHours = { 0,0 };
				par<p4, p4> tz_abbr = { 0,0 };
				par<p4, p4> tz_abbrName = { 0,0 };
				par<p4, p4> tz_isDst = { 0,0 };
			};

			airport_parser(const char* data) : data_(data)
			{
				json::parse_value(data, json::parse_space(data, 0), *this);
			}

			void begin_array() { ++arr_level_; }
			void end_array() { --arr_level_; }
			void begin_object() { ++obj_level_; }
			void end_object() 
			{ 
				--obj_level_; 
				if (1 == obj_level_)
				{
					if
						(
							rec.id.first < obj_position_
							|| rec.name.first < obj_position_
							|| rec.iata.first < obj_position_
							|| rec.icao.first < obj_position_
							|| rec.city.first < obj_position_
							|| rec.lat.first < obj_position_
							|| rec.lon.first < obj_position_
							|| rec.country.first < obj_position_
							|| rec.alt.first < obj_position_
							|| rec.size.first < obj_position_
							|| rec.tz_name.first < obj_position_
							|| rec.tz_offset.first < obj_position_
							|| rec.tz_offsetHours.first < obj_position_
							|| rec.tz_abbr.first < obj_position_
							|| rec.tz_abbrName.first < obj_position_
							|| rec.tz_isDst.first < obj_position_
							)
					{
						if (-1 == error_line_)
							error_line_ = line_;
					}
					else
					{
						lines_.push_back(rec);
					}
					obj_position_ = rec.tz_isDst.second;
					++line_;
				}
			}

			void text(json::type type, p4 b, p4 e)
			{
				if (1 != arr_level_)
					return;
				
				if (2 == obj_level_)
				{
					if (add("id", rec.id, b, e))
						return;
					if (add("name", rec.name, b, e))
						return;
					if (add("iata", rec.iata, b, e))
						return;
					if (add("icao", rec.icao, b, e))
						return;
					if (add("city", rec.city, b, e))
						return;
					if (add("lat", rec.lat, b, e))
						return;
					if (add("lon", rec.lon, b, e))
						return;
					if (add("country", rec.country, b, e))
						return;
					if (add("alt", rec.alt, b, e))
						return;
					if (add("size", rec.size, b, e))
						return;
					return;
				}

				if (3 != obj_level_)
					return;

				//tz
				if (add("name", rec.tz_name, b, e))
					return;
				if (add("offset", rec.tz_offset, b, e))
					return;
				if (add("offsetHours", rec.tz_offsetHours, b, e))
					return;
				if (add("abbr", rec.tz_abbr, b, e))
					return;
				if (add("abbrName", rec.tz_abbrName, b, e))
					return;
				if (add("isDst", rec.tz_isDst, b, e))
					return;
			}
			void key(p4 begin, p4 end)
			{
				key_b_ = begin;
				key_e_ = end;
			}

			arr<record> lines_;
			int error_line_ = -1;

			bool is_equal(const char* s, p4 b, p4 e)
			{
				return !strncmp(data_ + b, s, e - b);
			}
		private:
			const char* data_;
			int arr_level_ = 0;
			int obj_level_ = 0;
			p4 key_b_ = 0;
			p4 key_e_ = 0;
			p4 obj_position_ = 0;
			p4 line_ = 0;

			record rec;

			inline bool add(const char* test, par<p4, p4>& arr, p4 b, p4 e)
			{
				if (arr.first > obj_position_)
					return false;
				if (!is_equal(test, key_b_, key_e_))
					return false;

				arr.first = b;
				arr.second = e;

				return true;
			}
		};

		struct airports
		{
			airports(const char* data, const arr<airport_parser::record>& lines)
			{
				std::for_each(lines.cbegin(), lines.cend(), [this, data](const airport_parser::record& line)
				{
					id_.push_back(convert_p4(data + line.id.first, data + line.id.second));
					size_.push_back(convert_p4(data + line.size.first, data + line.size.second));
					//tz_offset_.push_back(convert_p4(data + line.tz_offset.first, data + line.tz_offset.second));
					tz_isDst_.push_back(str(data + line.tz_isDst.first, data + line.tz_isDst.second) == "true" ? 1 : 0);

					name_.push_back(full_trim(json_unescape(data + line.name.first, data + line.name.second)));
					iata_.push_back(full_trim(json_unescape(data + line.iata.first, data + line.iata.second)));
					icao_.push_back(full_trim(json_unescape(data + line.icao.first, data + line.icao.second)));
					city_.push_back(full_trim(json_unescape(data + line.city.first, data + line.city.second)));
					country_.push_back(full_trim(json_unescape(data + line.country.first, data + line.country.second)));
					tz_name_.push_back(full_trim(json_unescape(data + line.tz_name.first, data + line.tz_name.second)));
					tz_offsetHours_.push_back(full_trim(json_unescape(data + line.tz_offsetHours.first, data + line.tz_offsetHours.second)));
					tz_abbr_.push_back(full_trim(json_unescape(data + line.tz_abbr.first, data + line.tz_abbr.second)));
					tz_abbrName_.push_back(full_trim(json_unescape(data + line.tz_abbrName.first, data + line.tz_abbrName.second)));
				});
			}

			arr<par<n4,par<str, p4>>> name_stats() const { return name_.stats_by_count(); }
			arr<par<str, p4>> iata_stats() const { return iata_.stats(); }
			arr<par<str, p4>> icao_stats() const { return icao_.stats(); }
			arr<par<str, p4>> city_stats() const { return city_.stats(); }
			arr<par<str, p4>> country_stats() const { return country_.stats(); }
			arr<par<str, p4>> tz_name_stats() const { return tz_name_.stats(); }
			arr<par<str, p4>> tz_offsetHours_stats() const { return tz_offsetHours_.stats(); }
			arr<par<str, p4>> tz_abbr_stats() const { return tz_abbr_.stats(); }
			arr<par<str, p4>> tz_abbrName_stats() const { return tz_abbrName_.stats(); }

			arr<sz> indexes_from_name(p4 name_id) const
			{
				return name_.indexes_from_id(name_id);
			}
			
			pack<p4> id_;
			lookup_field name_;
			lookup_field iata_;
			lookup_field icao_;
			lookup_field city_;
			//lat
			//lon
			lookup_field country_;
			//alt
			pack<p4> size_;
			lookup_field tz_name_;
			//pack<p4> tz_offset_;
			lookup_field tz_offsetHours_;
			lookup_field tz_abbr_;
			lookup_field tz_abbrName_;
			pack<p4> tz_isDst_;
		};
	}
}