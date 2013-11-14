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

#ifndef __Tippi__ConstructBehavior__
#define __Tippi__ConstructBehavior__

#include "SharedPointer.h"

#include <iostream>

namespace Tippi {
    namespace Behavior {
        class Automaton;
    }
    
    namespace Interval {
        class Net;
    }
    
    struct ConstructBehavior {
        typedef std::tr1::shared_ptr<Interval::Net> NetPtr;
        typedef std::tr1::shared_ptr<Behavior::Automaton> BehPtr;

        BehPtr operator()(NetPtr net) const;
    };
}

#endif /* defined(__Tippi__ConstructBehavior__) */
