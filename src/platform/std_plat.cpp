#include "plat.h"

using namespace std;

namespace ai
{
	n8 plat::cputicks()
	{
		return 0;
		//    return __builtin_readcyclecounter();
	}

	void plat::log_debug(const str&)
	{
	}
}
