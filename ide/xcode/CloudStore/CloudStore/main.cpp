//
//  main.cpp
//  CloudStore
//
//  Created by Hans Malherbe on 2018/08/05.
//  Copyright © 2018 AiNET. All rights reserved.
//

#include <iostream>
#include "portal.hpp"

int main(int argc, const char * argv[])
{
    auto portal = ai::Portal::create();
    // insert code here...
    std::cout << portal->ping() << "\n";
    return 0;
}
