#pragma once
#include "../../general.h"
#include "../../bridge/cpp/file_type.hpp"
#include <QImage>

namespace ai
{
	struct mainwindow
	{
		virtual void preview(const QImage&) = 0;
		virtual void reset() = 0;
	};

	struct mainwindow_register
	{
		virtual void register_host(mainwindow&) = 0;
		virtual void deregister_host(mainwindow&) = 0;
	};
}