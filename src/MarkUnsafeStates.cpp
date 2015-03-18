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

#include "MarkUnsafeStates.h"

namespace Tippi {
    ClosureAutomaton::Ptr MarkUnsafeStates::operator()(ClosureAutomaton::Ptr automaton) const {
        const ClosureAutomaton::StateSet& states = automaton->getStates();
        ClosureState::resetVisited(states.begin(), states.end());
        
        ClosureAutomaton::StateSet markedStates = findInitialUnsafeStates(states);
        while (!markedStates.empty())
            markedStates = markPredecessors(markedStates);
        
        return automaton;
    }
    
    ClosureAutomaton::StateSet MarkUnsafeStates::findInitialUnsafeStates(const ClosureAutomaton::StateSet& states) const {
        ClosureAutomaton::StateSet result;
        
        ClosureAutomaton::StateSet::iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClosureState* state = *it;
            const Closure& closure = state->getClosure();
            if (state->isDeadlock()) {
                if (closure.containsBoundViolation() || closure.containsLoop()) {
                    state->setSafe(false);
                    result.insert(state);
                }
            }
        }
        
        return result;
    }
    
    ClosureAutomaton::StateSet MarkUnsafeStates::markPredecessors(const ClosureAutomaton::StateSet& states) const {
        ClosureAutomaton::StateSet result;

        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClosureAutomaton::State* state = *it;
            markStates(result, findUndeterminedPredecessors(state));
        }
        
        return result;
    }
    
    void MarkUnsafeStates::markStates(ClosureAutomaton::StateSet& markedStates, const ClosureAutomaton::StateSet& unmarkedStates) const {
        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = unmarkedStates.begin(), end = unmarkedStates.end(); it != end; ++it) {
            ClosureAutomaton::State* predecessor = *it;
            if (determineSafety(predecessor))
                markedStates.insert(predecessor);
        }
    }

    ClosureAutomaton::StateSet MarkUnsafeStates::findUndeterminedPredecessors(const ClosureAutomaton::State* state) const {
        ClosureAutomaton::StateSet result;
        
        const ClosureAutomaton::Edge::List& incoming = state->getIncoming();
        ClosureAutomaton::Edge::List::const_iterator it, end;
        for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
            ClosureAutomaton::Edge* edge = *it;
            ClosureAutomaton::State* predecessor = edge->getSource();
            if (!predecessor->isSafetyKnown())
                result.insert(predecessor);
        }
        return result;
    }
    
    bool MarkUnsafeStates::determineSafety(ClosureAutomaton::State* state) const {
        assert(!state->isSafetyKnown());
        
        static const size_t D = Interval::NetState::DisabledTransition;
        
        const Interval::NetState netState(Marking::createMarking(1, 0, 0, 1, 1, 0),
                                    Marking::createMarking(1, 1, D, 0, 0, D));
        Closure example;
        example.addState(netState);
        
        const Closure& closure = state->getClosure();
        
        if (closure.containsBoundViolation() || closure.containsLoop()) {
            state->setSafe(false);
            return true;
        }
        
        if (state->isFinal()) {
            state->setSafe(state->isStable());
            return true;
        }

        size_t serviceSuccessors = 0;
        size_t safeServiceSuccessors = 0;
        size_t unsafeServiceSuccessors = 0;
        size_t partnerSuccessors = 0;
        size_t safePartnerSuccessors = 0;
        size_t unknownPartnerSuccessors = 0;
        bool hasTimeEdge = false;
        bool hasUnknownTimeSuccessor = false;
        bool hasSafeTimeSuccessor = false;
        
        const ClosureEdge::List& outgoing = state->getOutgoing();
        ClosureEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end();
             it != end;
             ++it) {
            ClosureEdge* edge = *it;
            ClosureState* successor = edge->getTarget();
            
            if (edge->isLoop()) {
                hasTimeEdge = true;
            } else {
                if (!successor->isEmpty()) {
                    if (edge->isServiceAction()) {
                        ++serviceSuccessors;
                        if (successor->isSafetyKnown()) {
                            if (successor->isSafe())
                                ++safeServiceSuccessors;
                            else
                                ++unsafeServiceSuccessors;
                        }
                    } else if (edge->isPartnerAction()) {
                        ++partnerSuccessors;
                        if (successor->isSafetyKnown()) {
                            if (successor->isSafe())
                                ++safePartnerSuccessors;
                        } else {
                            ++unknownPartnerSuccessors;
                        }
                    } else {
                        hasTimeEdge = true;
                        if (successor->isSafetyKnown()) {
                            if (successor->isSafe())
                                hasSafeTimeSuccessor = true;
                        } else {
                            hasUnknownTimeSuccessor = true;
                        }
                    }
                }
            }
        }
        
        if (unsafeServiceSuccessors > 0) {
            state->setSafe(false);
            return true;
        }
        
        if ((unknownPartnerSuccessors == 0 && safePartnerSuccessors == 0) &&
            (hasTimeEdge && !hasUnknownTimeSuccessor && !hasSafeTimeSuccessor)) {
            state->setSafe(false);
            return true;
        }

        if (safeServiceSuccessors == serviceSuccessors) {
            if (safePartnerSuccessors > 0 || !hasTimeEdge || hasSafeTimeSuccessor) {
                state->setSafe(true);
                return true;
            }
        }
        
        return false;
    }
}
