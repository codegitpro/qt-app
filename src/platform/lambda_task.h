#pragma once
#include "../general.h"
#include "../bridge/cpp/task.hpp"

namespace ai
{
	struct lambda_task : ai::Task
	{
		lambda_task(funk<void()> fun) : fun_(fun) {}
		void run() final
		{
			fun_();
		}
	private:
		funk<void()> fun_;
	};
}
