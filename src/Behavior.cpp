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
    BehaviorEdge::BehaviorEdge(BehaviorState* source, BehaviorState* target, const String& label) :
    AutomatonEdge(source, target, label) {}
    
    BehaviorEdge::BehaviorEdge(BehaviorState* source, BehaviorState* target) :
    AutomatonEdge(source, target) {}

    BehaviorState::Key::Key(const Interval::NetState& i_netState, const bool i_boundViolation) :
    netState(i_netState),
    boundViolation(i_boundViolation) {}

    BehaviorState::Key::Key(const BehaviorState* state) :
    netState(state->getNetState()),
    boundViolation(state->isBoundViolation()) {}

    int BehaviorState::KeyCmp::operator() (const Key& lhs, const Key& rhs) const {
        if (lhs.boundViolation) {
            if (rhs.boundViolation)
                return 0;
            return -1;
        } else if (rhs.boundViolation) {
            return 1;
        }
        return lhs.netState.compare(rhs.netState);
    }

    BehaviorState::BehaviorState(const Interval::NetState& netState) :
    m_netState(netState),
    m_boundViolation(false) {}
    
    BehaviorState::BehaviorState() :
    m_netState(0, 0),
    m_boundViolation(true) {}
    
    const BehaviorState::Key BehaviorState::getKey(const BehaviorState* state) {
        return Key(state->getNetState(), state->isBoundViolation());
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
    
    BehaviorState* Behavior::findOrCreateBoundViolationState() {
        try {
            if (m_boundViolationState == NULL)
                m_boundViolationState = createState();
            return m_boundViolationState;
        } catch (...) {
            delete m_boundViolationState;
            m_boundViolationState = NULL;
            throw;
        }
    }
}
