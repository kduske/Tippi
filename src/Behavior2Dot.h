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

#ifndef __Tippi__Behavior2Dot__
#define __Tippi__Behavior2Dot__

#include "Behavior.h"
#include <iostream>

namespace Tippi {
    struct Behavior2Dot {
        void operator()(const Behavior::Ptr behavior, std::ostream& stream);
    };
}

#endif /* defined(__Tippi__Behavior2Dot__) */
