#pragma once
#include "../test/libuv/http_request.h"
#include "json.h"
#include "zz_algorithms.h"
#include "../test/libuv/file_writeall.h"
#include <iostream>
#include "date.h"

namespace ai
{
	namespace fr24
	{
		struct feed_parser
		{
			feed_parser(const char* data) : data_(data)
			{
				json::parse_value(data, json::parse_space(data, 0), *this);
			}

			void begin_array() { ++level_; }
			void end_array()
			{
				--level_;
				if (1 == level_ && phrases_.size() % 19 != 0 && error_line_ == -1)
					error_line_ = static_cast<int>(phrases_.size() / 19);
			}
			void begin_object() {}

			void end_object()
			{
			}

			void text(json::type type, p4 begin, p4 end)
			{
				if (2 != level_)
					return;
				phrases_.push_back({ begin, end });
			}
			void key(p4 begin, p4 end)
			{
			}

			const arr<par<p4, p4>>& phrases() const { return phrases_; }
			int error_line() const { return error_line_; }
		private:
			const char* data_;

			arr<par<p4, p4>> phrases_;

			int error_line_ = -1;

			int level_ = 0;
		};

		struct coord
		{
			int longitude;
			int latitude;
		};
		struct box
		{
			coord lower_left;
			coord upper_right;
			str to_str() const
			{
				return std::to_string(upper_right.latitude) + ","
					+ std::to_string(lower_left.latitude) + ","
					+ std::to_string(lower_left.longitude) + ","
					+ std::to_string(upper_right.longitude);
			}
		};
		struct feed
		{
			struct response
			{
				p4 status_code;
				arr<Header> headers;
				str body;
			};

			static void scan_inc(uv_loop_t& loop, str cookie, int sequence, arr<response> responses, funk<void(arr<response>)> fun)
			{
				if (sequence >= 36)
				{
					fun(std::move(responses));
					return;
				}
				read_longitude(loop, cookie, sequence, [&loop, cookie, sequence, fun, responses](p4 sc, auto&& headers, str data) mutable
				{
					responses.push_back({ sc, headers, data });
					if (sc < 200 || 300 <= sc)
					{
						fun(responses);
						return;
					}
					scan_inc(loop, cookie, sequence + 1, responses, fun);
				});
			}

			static void scan(uv_loop_t& loop, str cookie, funk<void(arr<response>)> fun)
			{
				scan_inc(loop, cookie, 0, {}, fun);
			}

			static void read_longitude_and_log(uv_loop_t& loop, str cookie, int sequence, str path, funk<void()> fun)
			{
				read_longitude(loop, cookie, sequence, [&loop, cookie, sequence, fun, path](p4 sc, auto&& headers, str data)
				{
					auto meta = std::accumulate(headers.cbegin(), headers.cend(), str(), [&loop, cookie, sequence, path, fun](auto&& acc, const Header& header)
					{
						return acc + header.name + ": " + header.value + "\n";
					});
					auto headers_filename = path + "/" + lpad(std::to_string(sequence), '0', 2) + " " + std::to_string(sc) + " headers.txt";

					uv::file_writeall::writeall(loop, headers_filename, meta, [&loop, cookie, sequence, path, sc, data = std::move(data), headers_filename, fun](bool success)
					{
						if (!success)
						{
							std::cout << "cannot write " << headers_filename << "\n";
							return;
						}
						auto body_filename = path + "/" + lpad(std::to_string(sequence), '0', 2) + " " + std::to_string(sc) + " body.txt";
						uv::file_writeall::writeall(loop, body_filename, data, [&loop, cookie, sequence, path, body_filename, fun](bool success)
						{
							if (!success)
							{
								std::cout << "cannot write " << body_filename << "\n";
								return;
							}
							if (71 == sequence)
							{
								fun();
								return;
							}
							read_longitude_and_log(loop, cookie, sequence + 1, path, fun);
						});
					});
				});
			}

			static void scan_and_log_inc(uv_loop_t& loop, str cookie, str path, int scan_sequence)
			{
				auto filename = path + "/" + lpad(std::to_string(scan_sequence), '0', 3);
				read_longitude_and_log(loop, cookie, 0, filename, [&loop, cookie, path, scan_sequence]()
				{
					scan_and_log_inc(loop, cookie, path, scan_sequence + 1);
				});
			}

			static void scan_and_log(uv_loop_t& loop, str cookie, str path)
			{
				using namespace date;
				using namespace std::chrono;

				auto today = year_month_day{ floor<days>(system_clock::now()) };
				auto tp = system_clock::now();
				auto dp = floor<days>(tp);
				auto time = make_time(tp - dp);
				normalise_path(path);
				path += "/" + std::to_string(int(today.year())) + "-" + lpad(std::to_string(unsigned(today.month())), '0', 2) + "-" + lpad(std::to_string(unsigned(today.day())), '0', 2) + " ";
				path += lpad(std::to_string(time.hours().count()), ' ', 2) + "h" + lpad(std::to_string(time.minutes().count()), '0', 2) + lpad(std::to_string(time.seconds().count()), '0', 2);

				scan_and_log_inc(loop, cookie, path, 0);
			}

			static void read_longitude(uv_loop_t& loop, str cookie, int sequence, funk<void(p4 sc, const arr<Header>& headers, str body)> fun)
			{
				assert(sequence < 72);
				auto url = str("http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=90,-90,")
					+ std::to_string(sequence * 5 - 180)
					+ ","
					+ std::to_string(sequence * 5 - 175)//170,190
					+ "&adsb=1&mlat=1&flarm=1&faa=1&estimated=1&maxage=14400&air=1&gnd=1&vehicles=1&gliders=1&flags=17fff&limit=1500&array=1&client=ios_freemium&version=7.10.0";

				uv::http_request::create(loop, HttpVerb::GET, url, { {"Cookie", cookie}, {"User-Agent", "FlightradarFree/2018091302 CFNetwork/758.5.3 Darwin/15.6.0"} }, "", [fun](p4 code, auto&& headers, str data)
				{
					fun(code, headers, std::move(data));
				});
			}

			//http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=85,-85,-90,-80&adsb=1&mlat=1&flarm=1&faa=1&estimated=1&maxage=14400&air=1&gnd=1&vehicles=1&gliders=1&flags=17fdf&limit=700&array=1&client=ios_freemium&version=7.10.0

			static void read(uv_loop_t& loop, str cookie, box coords, funk<void(str)> fun)
			{
				auto url = str("http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=")
					+ coords.to_str()
					+ "&adsb=1&mlat=1&flarm=1&faa=1&estimated=1&maxage=14400&air=1&gnd=1&vehicles=1&gliders=1&flags=17fdf&limit=700&array=1&client=ios_freemium&version=7.10.0";
				//85,-85,-90,-80

				uv::http_request::create(loop, HttpVerb::GET, url, { {"Cookie", cookie}, {"User-Agent", "FlightradarFree/2018091302 CFNetwork/758.5.3 Darwin/15.6.0"} }, "", [fun](p4 code, auto headers, str data)
				{
					fun(std::move(data));
				});
			}
		};
	}
}