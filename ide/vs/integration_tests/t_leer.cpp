#include "catch.h"
#include "libuv/uv_plat.h"
#include <iostream>
#include "platform/std_task_scheduler.h"

using namespace std;

namespace ai
{
	
const char* test_file = "test_file.txt";

void copy_file(uv_plat& x, str path, str& control_content, bool& success)
{
	x.copy_file(path, test_file, [&x, &control_content, &success](bool transferred)
	{
		REQUIRE(transferred);
		x.file_readall(test_file, [&x, &control_content, &success](str s)
		{
			success = control_content.size() == s.size() && equal(s.cbegin(), s.cend(), control_content.cbegin());
			x.file_delete(test_file, [&](bool deleted)
			{
				REQUIRE(deleted);
				x.stop_taking_tasks();
			});
		});
	});
}


TEST_CASE("t_leer")
{
	cout << "t_leer\n";
	
	bool success = false;
	str control_content;

	{
		std_task_scheduler sched;
		uv_plat x(sched, R"(c:\temp\appdata)");
		//x.make_path(R"(c:\temp\hans\mal\her)", [](plat::status status)
		//{
		//});

		x.file_delete(test_file, [&x, &control_content, &success](bool)
		{
			x.file_size(test_file, [&x, &control_content, &success](bool size_found, sz)
			{
				REQUIRE(!size_found);
				auto path = x.current_working_directory() + "\\..\\..\\..\\json.h";
				x.file_readall(path, [&x, &control_content, path, &success](str s)
				{
					if (s.empty())
						return;
					control_content = s;
					copy_file(x, path, control_content, success);
				});
			});
		});
	}
	REQUIRE(6349 == control_content.size());
	REQUIRE(success);
	cout << "t_leer out\n";
}

const char* t_leer_transfer = "t_leer.transfer";

TEST_CASE("t_leer.transfer")
{
	cout << "t_leer.transfer\n";
	str content;

	{
		std_task_scheduler sched;
		uv_plat x(sched, R"(c:\temp\appdata)");
		x.file_delete(t_leer_transfer, [&x, &content](bool)
		{
			auto path = x.current_working_directory() + "\\..\\..\\..\\deps\\libuv1.22.0\\lib\\windows\\x64_static_crtdll\\release\\libuv.lib";
			x.file_readall(path, [&x, path, &content](str s)
			{
				content = s;
				REQUIRE(6407568 == content.size());
				x.copy_file(path, t_leer_transfer, [&x, &content](bool succeeded)
				{
					REQUIRE(succeeded);
					x.file_readall(t_leer_transfer, [&x, &content](str new_content)
					{
						REQUIRE(content.size() == new_content.size());
						REQUIRE(equal(content.cbegin(), content.cend(), new_content.cbegin()));
						x.file_delete(t_leer_transfer, [&x](bool)
						{
							x.stop_taking_tasks();
						});
					});
				});
			});
		});
	}
	REQUIRE(6407568 == content.size());
	cout << "t_leer.transfer out\n";
}

}