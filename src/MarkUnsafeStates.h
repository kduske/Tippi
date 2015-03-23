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

#ifndef __Tippi__MarkUnsafeStates__
#define __Tippi__MarkUnsafeStates__

#include "SharedPointer.h"
#include "Closure.h"

namespace Tippi {
    class MarkUnsafeStates {
    public:
        ClosureAutomaton::Ptr operator()(ClosureAutomaton::Ptr automaton) const;
    private:
        ClosureAutomaton::StateSet findInitialUnsafeStates(const ClosureAutomaton::StateSet& states) const;
        ClosureAutomaton::StateSet markPredecessors(const ClosureAutomaton::StateSet& unsafeStates) const;
        
        void markStates(ClosureAutomaton::StateSet& markedStates, const ClosureAutomaton::StateSet& unmarkedStates) const;
        ClosureAutomaton::StateSet findUndeterminedPredecessors(const ClosureAutomaton::State* state) const;
        
        bool determineSafety(ClosureAutomaton::State* state) const;

        ClosureAutomaton::ComponentList::const_iterator findDeadEndComponent(const ClosureAutomaton::ComponentList& components) const;
        bool isDeadEndComponent(const ClosureAutomaton::Component& component) const;
    };
}

#endif /* defined(__Tippi__MarkUnsafeStates__) */
