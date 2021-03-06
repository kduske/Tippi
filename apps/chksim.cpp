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
#include "SimpleAutomaton.h"
#include "SimpleAutomatonParser.h"

#include <getoptpp/getopt_pp.h>
#include <cassert>
#include <fstream>
#include <iostream>

static Tippi::SimpleAutomaton::Ptr parseAutomaton(const String& str) {
    using namespace Tippi;
    SimpleAutomatonParser parser(str);
    return SimpleAutomaton::Ptr(parser.parse());
}

static size_t findSplitOffset(const String& str) {
    const size_t first = str.find("AUTOMATON");
    if (first == std::string::npos || first + 9 < str.size())
        return first;
    return str.find("AUTOMATON", first + 9);
}

int main(int argc, const char* argv[]) {
    using namespace Tippi;
    using namespace GetOpt;

    bool weak = false;
    String simulatorPath;
    String simulateePath;
    GetOpt_pp ops(argc, argv);
    ops >> OptionPresent('w', "weak", weak);
    ops >> GlobalOption(simulatorPath);
    ops >> GlobalOption(simulateePath);
    
    SimpleAutomaton::Ptr simulator;
    SimpleAutomaton::Ptr simulatee;
    
    if (simulatorPath.empty() && simulateePath.empty()) {
        const String str((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
        const size_t splitAt = findSplitOffset(str);
        
        if (splitAt == String::npos) {
            std::cout << "Unable to parse input. Please provide two automata in text format." << std::endl;
            exit(1);
        }
        
        simulator = parseAutomaton(str.substr(0, splitAt));
        simulatee = parseAutomaton(str.substr(splitAt, str.size() - splitAt));
    } else {
        std::ifstream simulatorStream(simulatorPath.c_str());
        std::ifstream simulateeStream(simulateePath.c_str());
        const String simulatorStr((std::istreambuf_iterator<char>(simulatorStream)), std::istreambuf_iterator<char>());
        const String simulateeStr((std::istreambuf_iterator<char>(simulateeStream)), std::istreambuf_iterator<char>());

        simulator = parseAutomaton(simulatorStr);
        simulatee = parseAutomaton(simulateeStr);
    }
    
    if (weak) {
        if (simulator->weaklySimulates(*simulatee))
            std::cout << "First automaton weakly simulates second automaton" << std::endl;
        else
            std::cout << "No weak simulation relation found" << std::endl;
    } else {
        if (simulator->simulates(*simulatee))
            std::cout << "First automaton simulates second automaton" << std::endl;
        else
            std::cout << "No simulation relation found" << std::endl;
    }
}
