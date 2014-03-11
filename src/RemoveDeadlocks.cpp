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

#include "RemoveDeadlocks.h"

namespace Tippi {
    RemoveDeadlocks::ClPtr RemoveDeadlocks::operator()(ClPtr automaton) const {
        const ClState::Set& states = automaton->getStates();
        ClState::resetVisited(states.begin(), states.end());
        
        const ClState::Set deadlocks = findAndMarkPotentialDeadlocks(automaton);
        automaton->deleteStates(deadlocks.begin(), deadlocks.end());
        
        return automaton;
    }
    
    ClState::Set RemoveDeadlocks::findAndMarkPotentialDeadlocks(ClPtr automaton) const {
        ClState::Set potentialDeadlocks = findInitialDeadlocks(automaton);
        if (!potentialDeadlocks.empty()) {
            size_t previousSize;
            size_t iteration = 1;
            markDeadlockDistance(potentialDeadlocks, iteration);
            ++iteration;
            do {
                previousSize = potentialDeadlocks.size();
                const ClState::Set additionalDeadlocks = findAdditionalDeadlocks(potentialDeadlocks);
                markDeadlockDistance(additionalDeadlocks, iteration);
                potentialDeadlocks.insert(additionalDeadlocks.begin(), additionalDeadlocks.end());
                ++iteration;
            } while (previousSize < potentialDeadlocks.size());
            automaton->setMaxDeadlockDistnace(iteration - 1);
        }
        return potentialDeadlocks;
    }

    ClState::Set RemoveDeadlocks::findInitialDeadlocks(const ClPtr automaton) const {
        ClState::Set initialDeadlocks;
        
        const ClState::Set& states = automaton->getStates();
        ClState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClState* state = *it;
            if (state->isDeadlock()) {
                state->setVisited(true);
                initialDeadlocks.insert(state);
            }
        }
        
        return initialDeadlocks;
    }

    ClState::Set RemoveDeadlocks::findAdditionalDeadlocks(const ClState::Set& states) const {
        ClState::Set additional;
        
        const ClState::Set candidates = findDeadlockCandidates(states);
        ClState::Set::const_iterator it, end;
        for (it = candidates.begin(), end = candidates.end(); it != end; ++it) {
            ClState* state = *it;
            if (isPotentialDeadlock(state)) {
                // std::cout << "     Found additional deadlock: " << state->asString(",", ";") << std::endl;
                state->setVisited(true);
                additional.insert(state);
            }
        }
        
        return additional;
    }

    ClState::Set RemoveDeadlocks::findDeadlockCandidates(const ClState::Set& states) const {
        ClState::Set candidates;
        
        ClState::Set::const_iterator sIt, sEnd;
        for (sIt = states.begin(), sEnd = states.end(); sIt != sEnd; ++sIt) {
            ClState* state = *sIt;
            const ClEdge::List& incoming = state->getIncoming();
            ClEdge::List::const_iterator eIt, eEnd;
            for (eIt = incoming.begin(), eEnd = incoming.end(); eIt != eEnd; ++eIt) {
                ClEdge* edge = *eIt;
                ClState* predecessor = edge->getSource();
                if (predecessor != state) {
                    if (!predecessor->isVisited())
                        candidates.insert(predecessor);
                }
            }
        }
        
        
        return candidates;
    }
    
    bool RemoveDeadlocks::isPotentialDeadlock(const ClState* state) const {
        assert(!state->isVisited());
        assert(!state->getClosure().getStates().empty());
        
        if (state->isFinal())
            return false;
        
        // this is the number of edges labeld with an action from the partner alphabet
        size_t pc = 0;
        // this is the number of edges labeled with an action from the partner alphabet which lead to a potential DL or to the empty state
        size_t pdl = 0;
        // this indicates whether the state has an outgoing edge labeld with an action from the service alphabet or 1 which leads to a potential DL
        bool sdl = false;
        // this is the number of edges which lead to a potential DL, to the given state or to the empty state
        size_t odl = 0;
        
        const ClEdge::List& outgoing = state->getOutgoing();
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const ClEdge* edge = *it;
            const ClState* succ = edge->getTarget();
            const bool succDL = succ->isVisited();
            if (edge->isPartnerAction()) {
                ++pc;
                if (succDL || succ->isEmpty())
                    ++pdl;
            }
            sdl |= ((edge->isServiceAction() || edge->isTimeAction()) && succDL);
            if (succDL || succ == state || succ->isEmpty())
                ++odl;
        }

        return (pdl == pc && sdl) || (odl == outgoing.size());
    }

    void RemoveDeadlocks::markDeadlockDistance(const ClState::Set& states, size_t distance) const {
        ClState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClState* state = *it;
            if (state->getDeadlockDistance() == 0)
                state->setDeadlockDistance(distance);
        }
    }
}