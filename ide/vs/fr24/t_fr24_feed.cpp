#include "../test/catch.h"
#include "../test/libuv/loop_wrapper.h"
#include "../test/libuv/http_request.h"
#include "platform/std_task_scheduler.h"
#include "../test/libuv/file_readall.h"
#include "fr24_feed.h"

namespace ai
{
	namespace fr24
	{
		TEST_CASE("feed parse")
		{
			{
				std_task_scheduler sched;
				uv::loop_wrapper l(sched);

				uv::file_readall::readall(l.handle(), R"(C:\p\os\docs\clearsky\fr24\feed.json)", [&l](str src)
				{
					auto parser = feed_parser(src.c_str());

					l.stop_taking_tasks();
				});
			}
		}
	}
}
