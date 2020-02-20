#pragma once
#include "../../general.h"
#include "../../bridge/cpp/worker.hpp"
#include "../../bridge/cpp/task.hpp"
#include <QThread>

namespace ai
{
	struct qt_worker : Worker
	{
		void schedule(const sptr<Task> & todo) final
		{
			worker_ = QThread::create([this, todo]()
			{
				todo->run();
				worker_->deleteLater();
			});
			worker_->start();
		}
		virtual ~qt_worker()
		{
			worker_->wait();
			delete worker_;
		}
	private:
		QThread* worker_;
	};
}
