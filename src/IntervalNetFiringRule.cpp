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

#include "IntervalNetFiringRule.h"

#include "Exceptions.h"

namespace Tippi {
    namespace Interval {
        FiringRule::Closure::Closure() :
        m_containsLoop(false),
        m_containsBoundViolation(false) {}

        bool FiringRule::Closure::operator<(const Closure& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool FiringRule::Closure::operator==(const Closure& rhs) const {
            return compare(rhs) == 0;
        }
        
        int FiringRule::Closure::compare(const Closure& rhs) const {
            NetState::Set::const_iterator lit = m_states.begin();
            const NetState::Set::const_iterator lend = m_states.end();
            NetState::Set::const_iterator rit = rhs.m_states.begin();
            const NetState::Set::const_iterator rend = rhs.m_states.end();
            
            while (lit != lend && rit != rend) {
                const NetState& lstate = *lit;
                const NetState& rstate = *rit;
                const int cmp = lstate.compare(rstate);
                if (cmp < 0)
                    return -1;
                if (cmp > 0)
                    return 1;
                ++lit;
                ++rit;
            }
            if (lit != lend)
                return 1;
            if (rit != rend)
                return -1;
            return 0;
        }

        bool FiringRule::Closure::isEmpty() const {
            return m_states.empty();
        }

        bool FiringRule::Closure::containsState(const NetState& state) const {
            return m_states.count(state) > 0;
        }

        const NetState::Set& FiringRule::Closure::getStates() const {
            return m_states;
        }
        
        bool FiringRule::Closure::containsLoop() const {
            return m_containsLoop;
        }
        
        bool FiringRule::Closure::containsBoundViolation() const {
            return m_containsBoundViolation;
        }

        bool FiringRule::Closure::addState(const NetState& state) {
            return m_states.insert(state).second;
        }

        void FiringRule::Closure::setContainsLoop() {
            m_containsLoop = true;
        }
        
        void FiringRule::Closure::setContainsBoundViolation() {
            m_containsBoundViolation = true;
        }

        String FiringRule::Closure::asString(const String& markingSeparator, const String& stateSeparator) const {
            StringStream result;
            NetState::Set::const_iterator it, end;
            for (it = m_states.begin(), end = m_states.end(); it != end; ++it) {
                const NetState& state = *it;
                result << state.asString(markingSeparator);
                if (std::distance(it, end) > 1)
                    result << stateSeparator;
            }
            return result.str();
        }
        
        FiringRule::FiringRule(const Net& net) :
        m_net(net) {}
        
        Transition::List FiringRule::getFireableTransitions(const NetState& state) const {
            Transition::List result;
            
            const Transition::List& transitions = m_net.getTransitions();
            Transition::List::const_iterator it, end;
            for (it = transitions.begin(), end = transitions.end(); it != end; ++it) {
                Transition* transition = *it;
                if (isFireable(transition, state))
                    result.push_back(transition);
            }
            return result;
        }
        
        bool FiringRule::isFireable(const Transition* transition, const NetState& state) const {
            return state.isPlaceEnabled(transition) && state.isTimeEnabled(transition);
        }
        
        NetState FiringRule::fireTransition(const Transition* transition, const NetState& state) const {
            assert(transition != NULL);
            assert(m_net.findTransition(transition->getName()) == transition);

            if (!isFireable(transition, state))
                throw FiringRuleException("Transition '" + transition->getName() + "' is not fireable");

            NetState newState(state);
            updateTokens(transition, newState);
            updateSiblings(transition, newState);
            updateSuccessors(transition, newState);
            return newState;
        }

        bool FiringRule::canMakeTimeStep(const NetState& state) const {
            const Transition::List& transitions = m_net.getTransitions();
            Transition::List::const_iterator it, end;
            for (it = transitions.begin(), end = transitions.end(); it != end; ++it) {
                Transition* transition = *it;
                if (!state.canMakeTimeStep(1, transition))
                    return false;
            }
            return true;
        }

        NetState FiringRule::makeTimeStep(const NetState& state) const {
            assert(canMakeTimeStep(state));
            
            NetState newState(state);
            const Transition::List& transitions = m_net.getTransitions();
            Transition::List::const_iterator it, end;
            for (it = transitions.begin(), end = transitions.end(); it != end; ++it) {
                Transition* transition = *it;
                newState.makeTimeStep(1, transition);
            }
            return newState;
        }

        FiringRule::Closure FiringRule::buildClosure(const NetState& state, const StringList& labels) const {
            Closure closure;
            buildClosureRecurse(state, labels, closure);
            return closure;
        }

        FiringRule::Closure FiringRule::buildClosure(const Closure& closure, const StringList& labels) const {
            return buildClosure(closure.getStates(), labels);
        }

        FiringRule::Closure FiringRule::buildClosure(const NetState::Set& states, const StringList& labels) const {
            Closure closure;
            NetState::Set::const_iterator it, end;
            for (it = states.begin(), end = states.end(); it != end && !closure.containsBoundViolation(); ++it) {
                const NetState& state = *it;
                buildClosureRecurse(state, labels, closure);
            }
            return closure;
        }

        void FiringRule::updateTokens(const Transition* transition, NetState& state) const {
            consumeTokens(transition, state);
            produceTokens(transition, state);
        }
        
        void FiringRule::consumeTokens(const Transition* transition, NetState& state) const {
            const Transition::IncomingList& incoming = transition->getIncoming();
            Transition::IncomingList::const_iterator it, end;
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                const PlaceToTransition* edge = *it;
                const Place* place = edge->getSource();
                assert(state.getPlaceMarking(place) >= edge->getMultiplicity());
                state.updatePlaceMarking(place, state.getPlaceMarking(place) - edge->getMultiplicity());
            }
        }
        
        void FiringRule::produceTokens(const Transition* transition, NetState& state) const {
            const Transition::OutgoingList& outgoing = transition->getOutgoing();
            Transition::OutgoingList::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const TransitionToPlace* edge = *it;
                const Place* place = edge->getTarget();
                state.updatePlaceMarking(place, state.getPlaceMarking(place) + edge->getMultiplicity());
            }
        }
        
        void FiringRule::updateSiblings(const Transition* transition, NetState& state) const {
            const Transition::IncomingList& incoming = transition->getIncoming();
            Transition::IncomingList::const_iterator it, end;
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                const PlaceToTransition* edge = *it;
                const Place* place = edge->getSource();
                resetPostset(place, state);
                enablePostset(place, state);
            }
        }
        
        void FiringRule::updateSuccessors(const Transition* transition, NetState& state) const {
            const Transition::OutgoingList& outgoing = transition->getOutgoing();
            Transition::OutgoingList::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const TransitionToPlace* edge = *it;
                const Place* place = edge->getTarget();
                enablePostset(place, state);
            }
        }

        void FiringRule::resetPostset(const Place* place, NetState& state) const {
            const Place::OutgoingList& outgoing = place->getOutgoing();
            Place::OutgoingList::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const PlaceToTransition* edge = *it;
                const Transition* transition = edge->getTarget();
                state.resetTransition(transition);
            }
        }
        
        void FiringRule::enablePostset(const Place* place, NetState& state) const {
            const Place::OutgoingList& outgoing = place->getOutgoing();
            Place::OutgoingList::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const PlaceToTransition* edge = *it;
                const Transition* transition = edge->getTarget();
                if (state.checkPlaceEnabled(transition)) {
                    if (!state.isPlaceEnabled(transition)) {
                        // transition was disabled, but now became enabled, so reset it
                        state.resetTransition(transition);
                    }
                } else {
                    if (state.isPlaceEnabled(transition)) {
                        // transition was enabled, but now became disabled
                        state.disableTransition(transition);
                    }
                }
            }
        }

        void FiringRule::buildClosureRecurse(const NetState& state, const StringList& labels, Closure& closure) const {
            if (!state.isBounded(m_net)) {
                closure.setContainsBoundViolation();
                return;
            }
            
            if (!closure.addState(state)) {
                closure.setContainsLoop();
                return;
            }

            const Transition::List fireableTransitions = getFireableTransitions(state);
            Transition::List::const_iterator it, end;
            for (it = fireableTransitions.begin(), end = fireableTransitions.end(); it != end && !closure.containsBoundViolation(); ++it) {
                const Transition* transition = *it;
                assert(state.checkPlaceEnabled(transition));
                assert(isFireable(transition, state));
                if (VectorUtils::contains(labels, transition->getLabel())) {
                    const NetState next = fireTransition(transition, state);
                    buildClosureRecurse(next, labels, closure);
                }
            }
        }
    }
}
