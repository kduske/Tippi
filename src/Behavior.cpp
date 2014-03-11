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
    class StateNetStateComparator {
    public:
        bool operator()(const BehaviorState* lhs, const BehaviorState* rhs) const {
            if (rhs == NULL)
                return false;
            if (lhs == NULL)
                return true;
            return lhs->getNetState() < rhs->getNetState();
        }
        
        bool operator()(const BehaviorState* lhs, const Interval::NetState& rhs) const {
            if (lhs == NULL)
                return true;
            return lhs->getNetState() < rhs;
        }
        
        bool operator()(const Interval::NetState& lhs, const BehaviorState* rhs) const {
            if (rhs == NULL)
                return false;
            return lhs < rhs->getNetState();
        }
        
        bool operator()(const Interval::NetState& lhs, const Interval::NetState& rhs) const {
            return lhs < rhs;
        }
    };
    
    BehaviorEdge::BehaviorEdge(BehaviorState* source, BehaviorState* target, const String& label, const bool tau) :
    AutomatonEdge<BehaviorState>(source, target, label, tau) {}
    
    bool BehaviorEdge::operator<(const BehaviorEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool BehaviorEdge::operator<(const BehaviorEdge* rhs) const {
        return compare(*rhs) < 0;
    }
    
    int BehaviorEdge::compare(const BehaviorEdge& rhs) const {
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
    
    BehaviorState::BehaviorState(const String& name, const Interval::NetState& netState) :
    AutomatonState(name),
    m_netState(netState),
    m_final(false),
    m_boundViolation(false) {}
    
    BehaviorState::BehaviorState(const String& name) :
    AutomatonState(name),
    m_netState(0, 0),
    m_final(false),
    m_boundViolation(true) {}
    
    bool BehaviorState::operator<(const BehaviorState& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool BehaviorState::operator<(const BehaviorState* rhs) const {
        return compare(*rhs) < 0;
    }
    
    int BehaviorState::compare(const BehaviorState& rhs) const {
        if (m_boundViolation) {
            if (rhs.m_boundViolation)
                return 0;
            return -1;
        } else if (rhs.m_boundViolation)
            return 1;
        return m_netState.compare(rhs.m_netState);
    }
    
    const Interval::NetState& BehaviorState::getNetState() const {
        return m_netState;
    }
    
    bool BehaviorState::isFinal() const {
        return m_final;
    }
    
    void BehaviorState::setFinal(bool final) {
        m_final = final;
    }
    
    bool BehaviorState::isBoundViolation() const {
        return m_boundViolation;
    }
    
    String BehaviorState::asString(const String separator) const {
        if (m_boundViolation)
            return "!";
        return m_netState.asString(separator);
    }
    
    Behavior::Behavior() :
    m_stateIndex(0),
    m_boundViolationState(NULL) {}
    
    Behavior::~Behavior() {
        SetUtils::clearAndDelete(m_states);
        SetUtils::clearAndDelete(m_edges);
        m_initialState = NULL;
        m_finalStates.clear();
    }
    
    BehaviorState* Behavior::createState(const Interval::NetState& netState) {
        State* state = new State(makeStateName(), netState);
        State::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            throw AutomatonException("Behavior already contains a state with net state '" + netState.asString() + "'");
        }
        m_states.insert(it, state);
        return state;
    }
    
    std::pair<BehaviorState*, bool> Behavior::findOrCreateState(const Interval::NetState& netState) {
        State* state = new State(makeStateName(), netState);
        State::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            return std::make_pair(*it, false);
        }
        m_states.insert(it, state);
        return std::make_pair(state, true);
    }
    
    BehaviorState* Behavior::findOrCreateBoundViolationState() {
        if (m_boundViolationState == NULL) {
            m_boundViolationState = new State("!!!");
            m_states.insert(m_boundViolationState);
        }
        return m_boundViolationState;
    }

    String Behavior::makeStateName() {
        static StringStream str;
        str.str("");
        str << m_stateIndex++;
        return str.str();
    }
}
