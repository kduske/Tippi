/*
 Copyright (C) 2013-2014 Kristian Duske
 
 This file is part of Tippi.
 
 Tippi is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Tippi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Tippi. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Filter/ConstructBehavior.h"
#include "Filter/ConstructMaximalNet.h"
#include "Filter/LoadIntervalNet.h"
#include "Filter/RenderBehavior.h"

#include <getoptpp/getopt_pp.h>
#include <cassert>
#include <iostream>

int main(int argc, const char* argv[]) {
    using namespace Tippi;
    using namespace GetOpt;
    
    bool showBoundViolations = false;
    GetOpt_pp ops(argc, argv);
    ops >> OptionPresent('b', "showBoundViolations", showBoundViolations);
    
    LoadIntervalNet loader;
    ConstructMaximalNet maximal;
    ConstructBehavior behavior(showBoundViolations);
    RenderBehavior render;
    
    LoadIntervalNet::NetPtr net = loader(std::cin);
    render(behavior(maximal(net)), std::cout);
}
