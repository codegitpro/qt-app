#include "../../general.h"
#include "../plat.h"

p8 plat::cputicks()
{
    return 0;
}

void plat::log_debug(const str&)
{
	//	QMessageLogger().debug() << QString::fromStdString(msg);
}
