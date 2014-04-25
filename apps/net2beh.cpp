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

#include "StringUtils.h"
#include "ConstructBehavior.h"
#include "ConstructMaximalNet.h"
#include "LoadIntervalNet.h"
#include "Behavior.h"
#include "Behavior2Dot.h"
#include "Automaton2Text.h"

#include <getoptpp/getopt_pp.h>
#include <cassert>
#include <iostream>

static void printUsage() {
    std::cout << "Usage:" << std::endl;
}

int main(int argc, const char* argv[]) {
    using namespace Tippi;
    using namespace GetOpt;
    
    bool showBoundViolations = false;
    String format = "text";
    GetOpt_pp ops(argc, argv);
    ops >> OptionPresent('b', "showBoundViolations", showBoundViolations);
    ops >> Option('f', "format", format);
    
    LoadIntervalNet loader;
    LoadIntervalNet::NetPtr net = loader(std::cin);

    ConstructMaximalNet maximal;
    ConstructBehavior behavior;
    if (showBoundViolations)
        behavior.createBoundViolationState();
    
    if (format == "text") {
        Automaton2Text render;
        Behavior::Ptr automaton = behavior(maximal(net));
        render(automaton.get(), std::cout);
    } else if (format == "dot") {
        Behavior2Dot render;
        render(behavior(maximal(net)), std::cout);
    } else {
        printUsage();
        exit(1);
    }
}
