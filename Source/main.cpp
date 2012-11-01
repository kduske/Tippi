//
//  main.cpp
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include <iostream>

#include "Behavior.h"
#include "ConstructBehavior.h"
#include "ConstructEnvironment.h"
#include "ConvertBehaviorToDot.h"
#include "FiringRule.h"
#include "LolaNetParser.h"
#include "Place.h"
#include "Transition.h"
#include "Net.h"

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, const char * argv[])
{
    std::fstream stream("../Data/test.net");
    assert(stream.is_open() && stream.good());
    
    Tippi::Lola::Parser parser(stream);

    Tippi::Net* net = parser.parseTimeNet();

    Tippi::FiringRule firingRule(*net);
    Tippi::Behavior behavior;
    
    (Tippi::ConstructEnvironment(*net))();
    Tippi::ConstructBehavior(*net, behavior, firingRule)();
    Tippi::ConvertBehaviorToDot(behavior, std::cout, "Behavior")();
    
    assert(net != NULL);
    delete net;
}

