//
//  main.cpp
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include <iostream>

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, const char * argv[])
{
    std::fstream stream("../Data/test2.net");
    assert(stream.is_open() && stream.good());
}

