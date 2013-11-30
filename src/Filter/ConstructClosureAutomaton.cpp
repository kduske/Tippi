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

#include "ConstructClosureAutomaton.h"

#include "Closure.h"
#include "Net/IntervalNetFiringRule.h"
#include "Net/IntervalNet.h"

#include <cassert>

namespace Tippi {
    ConstructClosureAutomaton::ClPtr ConstructClosureAutomaton::operator()(const NetPtr net) const {
        ClPtr automaton(new ClAutomaton());
        
        return automaton;
    }

    void ConstructClosureAutomaton::buildClosureRecurse(const NetPtr net, const Interval::FiringRule& rule, const Interval::NetState& netState, Closure::NetStateSet& states) const {
        if (!netState.isBounded(*net))
            return;
        
        const Interval::Transition::List fireableTransitions = rule.getFireableTransitions(netState);
        Interval::Transition::List::const_iterator it, end;
        for (it = fireableTransitions.begin(), end = fireableTransitions.end(); it != end; ++it) {
            const Interval::Transition* transition = *it;
        }
    }
    
    Closure ConstructClosureAutomaton::buildClosure(const NetPtr net, const Interval::FiringRule& rule, const Interval::NetState& netState) const {
        Closure::NetStateSet states;
        buildClosureRecurse(net, rule, netState, states);
        return Closure(states);
    }
}
