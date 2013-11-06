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

#include "Filter/ConstructMaximalNetFilter.h"
#include "Filter/LoadIntervalNetFilter.h"
#include "Filter/IntervalNetDotFilter.h"

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, const char * argv[]) {
    using namespace Tippi;
    
    std::fstream stream("data/test.net");
    assert(stream.is_open() && stream.good());
    
    LoadIntervalNetFilter loader;
    IntervalNetDotFilter output;
    ConstructMaximalNetFilter maximal;
    
    output(maximal(loader(stream)), std::cout);
}

