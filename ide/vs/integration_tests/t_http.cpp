#include "catch.h"
#include "openstack/openstack.h"
#include <iostream>
#include "libuv/uv_plat.h"
#include "libuv/loop_wrapper.h"
#include "libuv/http_request.h"
#include "platform/std_task_scheduler.h"

using namespace std;

namespace ai
{
	TEST_CASE("t_http_request")
	{
		std::cout << "t_http_request\n";

		p4 privacy_code = 0;
		p4 user_code = 0;

		{
			std_task_scheduler sched;
			uv::loop_wrapper l(sched);

			uv::http_request::create(l.handle(), HttpVerb::GET, "http://knysnaqa.comunity.me/privacy.html", {}, "", [&l, &privacy_code, &user_code](p4 code, auto headers, str data)
			{
				privacy_code = code;
				if (!code)
				{
					l.stop_taking_tasks();
					return;
				}
				uv::http_request::create(l.handle(), HttpVerb::GET, "http://knysnaqa.comunity.me/o/knysna/UserProfile", {}, "", [&l, &user_code](p4 code, auto, str data)
				{
					user_code = code;
					l.stop_taking_tasks();
				});
			});

		}

		REQUIRE(200 == privacy_code);
		REQUIRE(403 == user_code);
		std::cout << "t_http_request out\n";
	}
	
	TEST_CASE("t_http.openstack")
	{

		std::cout << "t_http.openstack\n";
		p4 suc = 0;

		{
			std_task_scheduler sched;
			uv_plat x(sched, R"(c:\temp\appdata)");
			openstack::session::connect(x, "testuser", "KidsnxUr4n22aLhj0u8a1y", "StorageTest", "http://205.134.171.234:5000/v2.0/tokens", [&x, &suc](uptr<openstack::session> session)
			{
				session->download_file("twentygb/a1.txt", R"(c:\temp\a1.txt)", 1, { 0x0dcc509a6f75849b, 0xc4ca4238a0b92382 }, [](auto) {}, [&x, &suc, session = session.get()](p4 sc, p8, p16)
				{
					if (200 != sc)
					{
						x.stop_taking_tasks();
						return;
					}

					auto progress_fun = [](Progress pr)
					{
						std::cout << pr.count << "\n";
					};
					session->download_file("twentygb/a/hamster.jpg", R"(c:\temp\hamster.jpg)", 0, { 0,0 }, progress_fun, [&x, &suc, progress_fun, session](p4 sc, p8, p16)
					{
						if (200 != sc)
						{
							x.stop_taking_tasks();
							return;
						}
						//session->upload_file("twentygb/a/20180818.txt", R"(c:\temp\20180818.txt)", { 0,0 }, progress_fun, [&x](auto code)
						//{
						//	x.stop_taking_tasks();
						//});
						suc = sc;
						session->download_file("twentygb/idea.jpg", R"(c:\temp\idea.jpg)", 0, { 0,0 }, progress_fun, [&x](p4 sc, p8, p16)
						{
							x.stop_taking_tasks();
						});
					});
				});
				session.release();
			});
		}

		REQUIRE(suc);
		std::cout << "t_http.openstack out\n";
	}
}