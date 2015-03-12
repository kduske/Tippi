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
#include "Automaton2Text.h"

#include <getoptpp/getopt_pp.h>
#include <cassert>
#include <fstream>
#include <iostream>

void printUsage() {
    std::cout << "Usage:" << std::endl;
}

int main(int argc, const char* argv[]) {
    using namespace Tippi;
    using namespace GetOpt;
    
    bool useInputFile = false;
    String filePath;
    bool keepDeadlocks = false;
    bool showEmptyState = false;
    bool showSCCs = false;
    String format = "text";
    GetOpt_pp ops(argc, argv);
    useInputFile = (ops >> Option('i', "inputFile", filePath));
    ops >> OptionPresent('d', "keepDeadlocks", keepDeadlocks);
    ops >> OptionPresent('e', "showEmptyState", showEmptyState);
    ops >> OptionPresent('s', "showSCCs", showSCCs);
    ops >> Option('f', "format", format);
    
    LoadIntervalNet::NetPtr net;
    
    if (useInputFile) {
        std::ifstream fileStream(filePath.c_str());
        if (!fileStream.is_open()) {
            std::cout << "Cannot open file: " << filePath << std::endl;
            exit(1);
        }
        
        LoadIntervalNet loader;
        net = loader(fileStream);
    } else {
        LoadIntervalNet loader;
        net = loader(std::cin);
    }
    
    ConstructMaximalNet maximal;
    ConstructClosureAutomaton closure;
    ClosureAutomaton::Ptr cl = closure(maximal(net));
    if (!keepDeadlocks) {
        RemoveDeadlocks deadlocks;
        RemoveUnreachableStates unreachable;
        cl = deadlocks(cl);
        cl = unreachable(cl);
    }
    
    if (format == "text") {
        Automaton2Text render;
        render(cl.get(), std::cout);
    } else if (format == "dot") {
        RenderClosureAutomaton render(std::cout, showEmptyState, showSCCs);
        render(cl.get());
    } else {
        printUsage();
        exit(1);
    }
    
}

