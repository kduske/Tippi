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

#ifndef __Tippi__RenderClosureAutomaton__
#define __Tippi__RenderClosureAutomaton__

#include "Closure.h"

#include <iostream>

namespace Tippi {
    class ClAutomaton;
    class ReAutomaton;
    
    struct RenderClosureAutomaton {
    private:
        std::ostream& m_stream;
        bool m_showEmptyState;
        bool m_showSCCs;
    public:
        RenderClosureAutomaton(std::ostream& stream, bool showEmptyState, bool showSCCs);
        void operator()(const ClosureAutomaton* automaton);
    private:
        void printComponent(const ClosureAutomaton::Component& component, size_t index);
        bool containsOnlyEmptyState(const ClosureAutomaton::Component& component) const;
        
        void printState(const ClosureState* state);
        void printEdge(const ClosureEdge* edge);
        void printAttribute(const String& name, const String& value);
        void openAttribute(const String& name);
        void closeAttribute();
        void printColorAttribute(const String& name, size_t r, size_t g, size_t b);
    };
}

#endif /* defined(__Tippi__RenderClosureAutomaton__) */
