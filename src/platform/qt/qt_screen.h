#pragma once
#include "../../general.h"

namespace ai
{
	struct qt_screen
	{
		virtual ~qt_screen() {}
		virtual void show() = 0;
		virtual void hide() = 0;
		virtual str title() = 0;
	};
}