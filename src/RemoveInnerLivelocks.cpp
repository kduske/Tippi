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

#include "RemoveInnerLivelocks.h"

namespace Tippi {
    ClosureAutomaton::Ptr RemoveInnerLivelocks::operator()(ClosureAutomaton::Ptr automaton) const {
        const ClosureAutomaton::StateSet& states = automaton->getStates();
        ClosureAutomaton::StateList livelocks;
        
        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClosureAutomaton::State* state = *it;
            const Interval::FiringRule::Closure& closure = state->getClosure();
            if (closure.containsLoop())
                livelocks.push_back(state);
        }
        
        automaton->deleteStates(livelocks.begin(), livelocks.end());
        return automaton;
    }
}
