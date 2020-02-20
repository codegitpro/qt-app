#pragma once
#include "plat.h"

namespace ai
{
    void create_thumbnail(str path, plat& platform, funk<void(arr<p1>)> fun);
    str filename_extension(const str& name);
    str content_type_from_path(const str& path);
}
