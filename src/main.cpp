/*
 Copyright (C) 2013 Kristian Duske
 
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
#include "Filter/ConstructClosureAutomaton.h"
#include "Filter/ConstructMaximalNet.h"
#include "Filter/LoadIntervalNet.h"
#include "Filter/RemoveDeadlocks.h"
#include "Filter/RemoveUnreachableStates.h"
#include "Filter/RenderBehavior.h"
#include "Filter/RenderClosureAutomaton.h"
#include "Filter/RenderIntervalNet.h"

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, const char * argv[]) {
    using namespace Tippi;

    LoadIntervalNet loader;
    ConstructMaximalNet maximal;
    // ConstructBehavior behavior;
    ConstructClosureAutomaton closure;
    RemoveDeadlocks removeDeadlocks;
    RemoveUnreachableStates removeUnreachable;
    

   // RenderBehavior renderBehavior;
    RenderClosureAutomaton renderClosure;
//    RenderIntervalNet renderNet;
    
//    renderNet(maximal(loader(stream)), // std::cout);

    if (argc == 2) {
        std::fstream stream(argv[1]);
        assert(stream.is_open() && stream.good());
        
        renderClosure(removeUnreachable(removeDeadlocks(closure(maximal(loader(stream))))), std::cout);
    } else {
        renderClosure(removeUnreachable(removeDeadlocks(closure(maximal(loader(std::cin))))), std::cout);
    }
}

