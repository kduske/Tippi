//
//  main.cpp
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include <iostream>

#include "Place.h"
#include "Transition.h"

int main(int argc, const char * argv[])
{
    new TPNA::Place("awdsf");
    new TPNA::Transition("asdf");
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

