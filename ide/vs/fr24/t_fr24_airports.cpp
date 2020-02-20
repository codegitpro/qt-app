#include "../test/catch.h"
#include "../test/libuv/loop_wrapper.h"
#include "../test/libuv/http_request.h"
#include "platform/std_task_scheduler.h"
#include "../test/libuv/file_readall.h"
#include "fr24_airports.h"
#include <iostream>

using namespace std;

namespace ai
{
	namespace fr24
	{
		TEST_CASE("airports parse")
		{
			{
				std_task_scheduler sched;
				uv::loop_wrapper l(sched);

				uv::file_readall::readall(l.handle(), R"(C:\p\os\docs\clearsky\fr24\airport.json)", [&l](str src)
				{
					auto parser = airport_parser(src.c_str());

					REQUIRE(-1 == parser.error_line_);
					REQUIRE(4174 == parser.lines_.size());

					{
						auto& line = parser.lines_[0];
						REQUIRE(parser.is_equal("2855", line.id.first, line.id.second));
						REQUIRE(parser.is_equal(R"(\tPraslin Island Airport)", line.name.first, line.name.second));
						REQUIRE(parser.is_equal(R"(PRI)", line.iata.first, line.iata.second));
						REQUIRE(parser.is_equal(R"(FSPP)", line.icao.first, line.icao.second));
						REQUIRE(parser.is_equal(R"(\tPraslin Island)", line.city.first, line.city.second));
						REQUIRE(parser.is_equal(R"(-4.31929)", line.lat.first, line.lat.second));
						REQUIRE(parser.is_equal(R"(55.69141)", line.lon.first, line.lon.second));
						REQUIRE(parser.is_equal(R"(Seychelles)", line.country.first, line.country.second));
						REQUIRE(parser.is_equal(R"(10)", line.alt.first, line.alt.second));
						REQUIRE(parser.is_equal(R"(528)", line.size.first, line.size.second));
						REQUIRE(parser.is_equal(R"(Indian\/Mahe)", line.tz_name.first, line.tz_name.second));
						REQUIRE(parser.is_equal(R"(14400)", line.tz_offset.first, line.tz_offset.second));
						REQUIRE(parser.is_equal(R"(4:00)", line.tz_offsetHours.first, line.tz_offsetHours.second));
						REQUIRE(parser.is_equal(R"(+04)", line.tz_abbr.first, line.tz_abbr.second));
						REQUIRE(parser.is_equal(R"(null)", line.tz_abbrName.first, line.tz_abbrName.second));
						REQUIRE(parser.is_equal(R"(false)", line.tz_isDst.first, line.tz_isDst.second));
					}

					{
						auto& line = parser.lines_[4173];
						REQUIRE(parser.is_equal("4843", line.id.first, line.id.second));
						REQUIRE(parser.is_equal(R"(Zyryanka Airport)", line.name.first, line.name.second));
						REQUIRE(parser.is_equal(R"(ZKP)", line.iata.first, line.iata.second));
						REQUIRE(parser.is_equal(R"(UESU)", line.icao.first, line.icao.second));
						REQUIRE(parser.is_equal(R"(Zyryanka)", line.city.first, line.city.second));
						REQUIRE(parser.is_equal(R"(65.738335)", line.lat.first, line.lat.second));
						REQUIRE(parser.is_equal(R"(150.707779)", line.lon.first, line.lon.second));
						REQUIRE(parser.is_equal(R"(Russia)", line.country.first, line.country.second));
						REQUIRE(parser.is_equal(R"(145)", line.alt.first, line.alt.second));
						REQUIRE(parser.is_equal(R"(30)", line.size.first, line.size.second));
						REQUIRE(parser.is_equal(R"(Asia\/Magadan)", line.tz_name.first, line.tz_name.second));
						REQUIRE(parser.is_equal(R"(39600)", line.tz_offset.first, line.tz_offset.second));
						REQUIRE(parser.is_equal(R"(11:00)", line.tz_offsetHours.first, line.tz_offsetHours.second));
						REQUIRE(parser.is_equal(R"(+11)", line.tz_abbr.first, line.tz_abbr.second));
						REQUIRE(parser.is_equal(R"(null)", line.tz_abbrName.first, line.tz_abbrName.second));
						REQUIRE(parser.is_equal(R"(false)", line.tz_isDst.first, line.tz_isDst.second));
					}

					/*
					auto ports = airports(src.c_str(), parser.lines_);

					auto names = ports.name_stats();
					auto iatas = ports.iata_stats();
					auto cities = ports.city_stats();
					auto contries = ports.country_stats();
					auto icaos = ports.icao_stats();
					auto tz_abbrNames = ports.tz_abbrName_stats();
					auto tz_abbr_stats = ports.tz_abbr_stats();
					auto tz_names = ports.tz_name_stats();
					auto tz_offsetHours = ports.tz_offsetHours_stats();

					find_if(names.cbegin(), names.cend(), [&ports](auto&& p)
					{
						auto indexes = ports.indexes_from_name(p.first);
						for_each(indexes.cbegin(), indexes.cend(), [&p, &ports](sz index)
						{
							cout << p.second.first << " " << ports.iata_.extract(index) << " " << ports.icao_.extract(index) << "\n";
						});
						return 1 == p.second.second;
					});
					*/
					l.stop_taking_tasks();
				});
			}

		}
	}
}