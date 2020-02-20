#pragma once
#include "general.h"
#include "../test/libuv/http_request.h"

namespace ai
{
	namespace fr24
	{
		struct login
		{
			static void create_session(uv_loop_t& loop, funk<void(str)> fun)
			{
				auto url = str("http://www.flightradar24.com/_json/mobileSettings.php?version=4&timestamp=0");
				uv::http_request::create(loop, HttpVerb::GET, url, { { "User-Agent", "FlightradarFree/2018091302 CFNetwork/758.5.3 Darwin/15.6.0" } }, "", [fun](p4 sc, const arr<Header>& headers, str data)
				{
					if (sc < 200 || 300 <= sc)
					{
						fun("");
						return;
					}

					auto session = std::accumulate(headers.cbegin(), headers.cend(), str(""), [](const str& acc, const Header& header)
					{
						if (header.name != "Set-Cookie")
							return acc;

						return cookie(header.value) + (acc.empty() ? "" : "; " + acc);
					});
					fun(session);
				});
			}

			static str cookie(const str& header)
			{
				return str(header.cbegin(), std::find(header.cbegin(), header.cend(), ';'));
			}
		};
	}
}