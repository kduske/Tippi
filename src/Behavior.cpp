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
    BehaviorEdge::BehaviorEdge(BehaviorState* source, BehaviorState* target, const String& label, const bool tau) :
    AutomatonEdge<BehaviorState>(source, target, label, tau) {}
    
    bool BehaviorEdge::operator<(const BehaviorEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool BehaviorEdge::operator<(const BehaviorEdge* rhs) const {
        return compare(*rhs) < 0;
    }
    
    BehaviorState::BehaviorState(const String& name, const Interval::NetState& netState) :
    AutomatonState(name),
    m_netState(netState),
    m_boundViolation(false) {}
    
    BehaviorState::BehaviorState(const String& name) :
    AutomatonState(name),
    m_netState(0, 0),
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
    
    bool BehaviorState::isBoundViolation() const {
        return m_boundViolation;
    }
    
    String BehaviorState::asString(const String separator) const {
        if (m_boundViolation)
            return "!";
        return m_netState.asString(separator);
    }
    
    Behavior::Behavior() :
    m_boundViolationState(NULL) {}
    
    BehaviorState* Behavior::createState(const Interval::NetState& netState) {
        State* state = new State(makeStateName(), netState);
        try {
            addState(state);
            return state;
        } catch (...) {
            delete state;
            throw;
        }
    }
    
    std::pair<BehaviorState*, bool> Behavior::findOrCreateState(const Interval::NetState& netState) {
        State* state = new State(makeStateName(), netState);
        return findOrAddState(state);
    }
    
    BehaviorState* Behavior::findOrCreateBoundViolationState() {
        try {
            if (m_boundViolationState == NULL) {
                m_boundViolationState = new State("!!!");
                addState(m_boundViolationState);
            }
            return m_boundViolationState;
        } catch (...) {
            delete m_boundViolationState;
            m_boundViolationState = NULL;
            throw;
        }
    }
    
    String Behavior::makeStateName() {
        static StringStream str;
        str.str("");
        str << m_stateIndex++;
        return str.str();
    }
}
