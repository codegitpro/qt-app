#define CATCH_CONFIG_RUNNER
#include "../test/catch.h"

#include <cstdio>
#include "../test/libuv/loop_wrapper.h"
#include "../test/libuv/http_request.h"
#include "platform/std_task_scheduler.h"
#include <iostream>
#include "fr24_feed.h"
#include "fr24_login.h"

using namespace std;
using namespace ai;

void test()
{
	{
		std_task_scheduler sched;
		uv::loop_wrapper l(sched);

		//fr24::feed::read(l.handle(), "FR24ID=ae08ch60gduhmn07rj8te6nmjseqsau6ufhmq2skdrv1589pm590; __cfduid=d97ecddd3dc2e4d55d076c8c7bfa7458e1537692709", { {-10, -50}, {100, 20} }, [](str data)
		//{

		//});
		fr24::login::create_session(l.handle(), [&l](str cookie)
		{
			//fr24::feed::scan(l.handle(), cookie, [](auto&& responses)
			//{

			//});
			fr24::feed::scan_and_log(l.handle(), cookie, R"(c:\temp\clearsky\feed)");
		});
		getchar();
		getchar();
	}

	
}

int main(int argc, char** argv)
{
	test();
	//Catch::Session().run(argc, argv);
	getchar();
	return 0;
}