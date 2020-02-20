#include "catch.h"
#include "libuv/uv_plat.h"
#include <iostream>
#include "caching_engine.h"
#include "platform/std_task_scheduler.h"

using namespace std;

namespace ai
{
	struct my_cb : cache_callback
	{
		my_cb(cache& c) : c_(c)
		{

		}
		void folder_change(n4 folder_id, const directory& dir) final
		{
			if (downloading_)
				return;

			auto files = dir.list_files(folder_id);
			auto a0 = find_if(files.cbegin(), files.cend(), [](auto&& item)
			{
				return "a0.txt" == item.name;
			});
			if (files.cend() != a0)
			{
				downloading_ = true;
				c_.download_file_to_cache(a0->id);
			}
			//for_each(files.cbegin(), files.cend(), [](auto&& item)
			//{
			//	cout << item.name << " \n";
			//});
		}
		void file_change(p4 file_id, const directory&) final
		{

		}
		void report(severity, const str&) final
		{

		}
		void preview_photo(p4 folder_id, p4 file_id, const str&, const directory&) final
		{

		}
		void preview_text(p4 folder_id, p4 file_id, const str&, const directory&) final
		{

		}
		void preview_file(p4 folder_id, p4 file_id, const str&, const directory&) final
		{

		}

	private:
		cache& c_;
		bool downloading_ = false;
	};

	TEST_CASE("t_cache")
	{

		//cout << "t_cache\n";
		//{
		//	std_task_scheduler sched;
		//	uv_plat x(sched, R"(c:\temp\appdata)");

		//	cache c(sched, x);
		//	my_cb cb(c);
		//	c.register_callback(cb);
		//	x.wait();
		//}
		//cout << "t_cache out\n";
	}
}
