#pragma once
#include "../bridge/cpp/os_gui.hpp"

namespace ai
{
	struct ui
	{
		static void enable(OsGui& gui, bool enabled)
		{
			for (int i = 1; ; ++i)
			{
				if (!gui.enable(i, enabled))
					return;
			}
		}
	};
}