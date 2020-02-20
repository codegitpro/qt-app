#pragma once
#include "../task_scheduler.h"
#include <QThread>

namespace ai
{
	struct qt_worker
	{
		qt_worker(funk<void()> fun)
		{
			worker_ = QThread::create([this, fun]()
			{
				fun();
				delete this;
			});
			worker_->start();
		}
		~qt_worker()
		{
			worker_->deleteLater();
		}
	private:
		QThread* worker_ = nullptr;
	};

	struct qt_task_scheduler : task_scheduler
	{
		qt_task_scheduler()
		{
		}

		void do_task(funk<void()> fun) final
		{
			new qt_worker(fun);
		}
	};
}
