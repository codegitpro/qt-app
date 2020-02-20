#pragma once
#include "../general.h"
#include "task_scheduler.h"
#include <thread>

namespace ai
{
	struct std_task_scheduler : task_scheduler
	{
		void do_task(funk<void()> fun) final
		{
			std::thread t(fun);
			t.detach();
		}
	};
}
