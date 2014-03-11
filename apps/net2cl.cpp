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

#include "ConstructClosureAutomaton.h"
#include "ConstructMaximalNet.h"
#include "ConstructRegionAutomaton.h"
#include "LoadIntervalNet.h"
#include "RemoveDeadlocks.h"
#include "RemoveUnreachableStates.h"
#include "RenderClosureAutomaton.h"

#include <getoptpp/getopt_pp.h>
#include <cassert>
#include <iostream>


int main(int argc, const char* argv[]) {
    using namespace Tippi;
    using namespace GetOpt;
    
    bool keepDeadlocks = false;
    bool showEmptyState = false;
    GetOpt_pp ops(argc, argv);
    ops >> OptionPresent('d', "keepDeadlocks", keepDeadlocks) >> OptionPresent('e', "showEmptyState", showEmptyState);
    
    LoadIntervalNet loader;
    ConstructMaximalNet maximal;
    ConstructClosureAutomaton closure;
    
    LoadIntervalNet::NetPtr net = loader(std::cin);
    ConstructClosureAutomaton::ClPtr cl = closure(maximal(net));
    if (!keepDeadlocks) {
        RemoveDeadlocks deadlocks;
        RemoveUnreachableStates unreachable;
        cl = deadlocks(cl);
        cl = unreachable(cl);
    }
    
    ConstructRegionAutomaton region;
    ConstructRegionAutomaton::RePtr re = region(cl);
    
    RenderClosureAutomaton render(showEmptyState);
    render(cl, re, std::cout);
}

