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

#include "Behavior.h"

#include "CollectionUtils.h"
#include "Exceptions.h"

#include <cassert>

namespace Tippi {
    namespace Behavior {
        class StateNetStateComparator {
        public:
            bool operator()(const State* lhs, const State* rhs) const {
                if (rhs == NULL)
                    return false;
                if (lhs == NULL)
                    return true;
                return lhs->getNetState() < rhs->getNetState();
            }
            
            bool operator()(const State* lhs, const Interval::NetState& rhs) const {
                if (lhs == NULL)
                    return true;
                return lhs->getNetState() < rhs;
            }
            
            bool operator()(const Interval::NetState& lhs, const State* rhs) const {
                if (rhs == NULL)
                    return false;
                return lhs < rhs->getNetState();
            }
            
            bool operator()(const Interval::NetState& lhs, const Interval::NetState& rhs) const {
                return lhs < rhs;
            }
        };

        Edge::Edge(State* source, State* target, const String& label, const bool tau) :
        AutomatonEdge<State>(source, target, label, tau) {}
        
        bool Edge::operator<(const Edge& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool Edge::operator<(const Edge* rhs) const {
            return compare(*rhs) < 0;
        }
        
        int Edge::compare(const Edge& rhs) const {
            const int sourceResult = getSource()->compare(*rhs.getSource());
            if (sourceResult < 0)
                return -1;
            if (sourceResult > 0)
                return 1;
            const int targetResult = getTarget()->compare(*rhs.getTarget());
            if (targetResult < 0)
                return -1;
            if (targetResult > 0)
                return 1;
            return m_label.compare(rhs.m_label);
        }

        State::State(const Interval::NetState& netState) :
        m_netState(netState),
        m_final(false),
        m_boundViolation(false) {}
        
        State::State() :
        m_netState(0, 0),
        m_final(false),
        m_boundViolation(true) {}
        
        bool State::operator<(const State& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool State::operator<(const State* rhs) const {
            return compare(*rhs) < 0;
        }
        
        int State::compare(const State& rhs) const {
            if (m_boundViolation) {
                if (rhs.m_boundViolation)
                    return 0;
                return -1;
            } else if (rhs.m_boundViolation)
                return 1;
            return m_netState.compare(rhs.m_netState);
        }
        
        const Interval::NetState& State::getNetState() const {
            return m_netState;
        }
        
        bool State::isFinal() const {
            return m_final;
        }
        
        void State::setFinal(bool final) {
            m_final = final;
        }
        
        bool State::isBoundViolation() const {
            return m_boundViolation;
        }
        
        const Behavior::State* State::getSuccessor(const String& edgeLabel) const {
            const OutgoingList& edges = getOutgoing();
            OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->getLabel() == edgeLabel)
                    return edge->getTarget();
            }
            return NULL;
        }
        
        String State::asString(const String separator) const {
            if (m_boundViolation)
                return "!";
            return m_netState.asString(separator);
        }
        
        Automaton::Automaton() :
        m_boundViolationState(NULL) {}
        
        Automaton::~Automaton() {
            SetUtils::clearAndDelete(m_states);
            SetUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        State* Automaton::createState(const Interval::NetState& netState) {
            State* state = new State(netState);
            State::Set::iterator it = m_states.lower_bound(state);
            if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
                delete state;
                throw AutomatonException("Behavior already contains a state with net state '" + netState.asString() + "'");
            }
            m_states.insert(it, state);
            return state;
        }
        
        std::pair<State*, bool> Automaton::findOrCreateState(const Interval::NetState& netState) {
            State* state = new State(netState);
            State::Set::iterator it = m_states.lower_bound(state);
            if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
                delete state;
                return std::make_pair(*it, false);
            }
            m_states.insert(it, state);
            return std::make_pair(state, true);
        }
        
        State* Automaton::findOrCreateBoundViolationState() {
            if (m_boundViolationState == NULL) {
                m_boundViolationState = new State();
                m_states.insert(m_boundViolationState);
            }
            return m_boundViolationState;
        }
    }
}
