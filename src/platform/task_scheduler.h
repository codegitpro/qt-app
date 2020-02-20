#pragma once
#include "../general.h"

namespace ai
{
	struct task_scheduler
	{
        virtual ~task_scheduler(){}
		virtual void do_task(funk<void()>) = 0;
	};
}
