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

#include "ReduceClosureAutomaton.h"

namespace Tippi {
    ReduceClosureAutomaton::ClPtr ReduceClosureAutomaton::operator()(ClPtr automaton) const {
        const ClState::Set& states = automaton->getStates();
        ClState::resetVisited(states.begin(), states.end());
        
        const ClState::Set potentialDeadlocks = findPotentialDeadlocks(automaton);
        automaton->deleteStates(potentialDeadlocks.begin(), potentialDeadlocks.end());
        
        const ClState::Set& unreachable = automaton->findUnreachableStates();
        automaton->deleteStates(unreachable.begin(), unreachable.end());
        
        return automaton;
    }

    ClState::Set ReduceClosureAutomaton::findPotentialDeadlocks(const ClPtr automaton) const {
        ClState::Set potentialDeadlocks = findInitialDeadlocks(automaton);
        size_t previousSize;
        // size_t iteration = 0;
        do {
            // std::cout << "====== Iteration " << iteration++ << " ======" << std::endl;
            previousSize = potentialDeadlocks.size();
            const ClState::Set additionalDeadlocks = findAdditionalDeadlocks(potentialDeadlocks);
            potentialDeadlocks.insert(additionalDeadlocks.begin(), additionalDeadlocks.end());
        } while (previousSize < potentialDeadlocks.size());
        return potentialDeadlocks;
    }

    ClState::Set ReduceClosureAutomaton::findInitialDeadlocks(const ClPtr automaton) const {
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

    ClState::Set ReduceClosureAutomaton::findAdditionalDeadlocks(const ClState::Set& states) const {
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

    ClState::Set ReduceClosureAutomaton::findDeadlockCandidates(const ClState::Set& states) const {
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

    bool ReduceClosureAutomaton::isPotentialDeadlock(const ClState* state) const {
        assert(!state->isVisited());
        assert(!state->getClosure().getStates().empty());
        
        if (state->isFinal())
            return false;
        
        size_t badEdges = 0;
        const ClEdge::List& outgoing = state->getOutgoing();
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const ClEdge* edge = *it;
            const ClState* succ = edge->getTarget();
            const bool succDL = succ->isVisited();
            if (edge->getType() == ClEdge::OutputSend || edge->getType() == ClEdge::InputRead) {
                if (succDL)
                    return true;
                ++badEdges;
            } else {
                if (succDL || succ == state || succ->getClosure().getStates().empty())
                    ++badEdges;
            }
        }
        
        return badEdges == outgoing.size();
    }
}
