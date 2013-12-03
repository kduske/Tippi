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

#ifndef __Tippi__RenderClosureAutomaton__
#define __Tippi__RenderClosureAutomaton__

#include "SharedPointer.h"

#include <iostream>

namespace Tippi {
    class ClAutomaton;
    class ReAutomaton;
    
    struct RenderClosureAutomaton {
        typedef std::tr1::shared_ptr<ClAutomaton> ClPtr;
        typedef std::tr1::shared_ptr<ReAutomaton> RePtr;

        void operator()(const ClPtr automaton, std::ostream& stream);
        void operator()(const ClPtr closureAutomaton, const RePtr regionAutomaton, std::ostream& stream);
    };
}

#endif /* defined(__Tippi__RenderClosureAutomaton__) */
