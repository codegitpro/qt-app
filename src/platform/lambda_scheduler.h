#pragma once
#include "../general.h"
#include "task_scheduler.h"
#include "lambda_task.h"
#include "../bridge/cpp/worker.hpp"

namespace ai
{
	struct lambda_scheduler : task_scheduler
	{
		lambda_scheduler(const sptr<ai::Worker>& w) : worker_(w)
		{

		}
		void do_task(funk<void()> fun) final
		{
			worker_->schedule(std::make_shared<lambda_task>(fun));
		}
	private:
		sptr<ai::Worker> worker_;
	};
}
