#include <intrin.h>
#include "../plat.h"
#include <Windows.h>

namespace ai
{
	p8 plat::cputicks()
	{
		return __rdtsc();
	}

	void plat::log_debug(const str& line)
	{
		OutputDebugStringA(line.c_str());
		OutputDebugStringA("\n");
		//	QMessageLogger().debug() << QString::fromStdString(msg);
	}
}
