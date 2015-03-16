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

#include "Closure.h"

#include "CollectionUtils.h"
#include "Exceptions.h"

#include <algorithm>
#include <cassert>

namespace Tippi {
    ClosureEdge::ClosureEdge(ClosureState* source, ClosureState* target, const String& label, const EdgeType type) :
    AutomatonEdge(source, target, label),
    m_type(type) {}
    
    ClosureEdge::EdgeType ClosureEdge::getType() const {
        return m_type;
    }
    
    bool ClosureEdge::isServiceAction() const {
        return m_type == EdgeType_OutputSend || m_type == EdgeType_InputRead;
    }
    
    bool ClosureEdge::isPartnerAction() const {
        return m_type == EdgeType_InputSend || m_type == EdgeType_OutputRead;
    }
    
    bool ClosureEdge::isTimeAction() const {
        return m_type == EdgeType_Time;
    }

    String ClosureEdge::asString() const {
        StringStream str;
        str << "{" << m_source->asString() << "} --" << m_label << "--> {" << m_target->asString() << "}";
        return str.str();
    }

    int ClosureState::KeyCmp::operator() (const Key& lhs, const Key& rhs) const {
        return lhs.compare(rhs);
    }

    ClosureState::ClosureState(const Closure& closure) :
    m_closure(closure),
    m_deadlockDistance(0),
    m_reachable(true) {}
    
    const ClosureState::Key& ClosureState::getKey(const ClosureState* state) {
        return state->getClosure();
    }

    const Closure& ClosureState::getClosure() const {
        return m_closure;
    }
    
    bool ClosureState::isEmpty() const {
        return m_closure.isEmpty();
    }

    bool ClosureState::isDeadlock() const {
        if (isFinal())
            return false;
        if (m_closure.getStates().empty())
            return false;
        
        const ClosureEdge::List& outgoing = getOutgoing();
        ClosureEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const ClosureEdge* edge = *it;
            const ClosureState* successor = edge->getTarget();
            if (successor != this && !successor->getClosure().getStates().empty())
                return false;
        }
        
        return true;
    }

    size_t ClosureState::getDeadlockDistance() const {
        return m_deadlockDistance;
    }
    
    void ClosureState::setDeadlockDistance(size_t deadlockDistance) {
        m_deadlockDistance = deadlockDistance;
    }

    bool ClosureState::isReachable() const {
        return m_reachable;
    }
    
    void ClosureState::setReachable(bool reachable) {
        m_reachable = reachable;
    }

    String ClosureState::asString() const {
        return asString(",", ",");
    }
    
    String ClosureState::asString(const String& markingSeparator, const String& stateSeparator) const {
        return m_closure.asString(markingSeparator, stateSeparator);
    }

    class StateCmp {
    public:
        bool operator()(const ClosureState* lhs, const ClosureState* rhs) const {
            return lhs < rhs;
        }
        bool operator()(const ClosureState* lhs, const Closure& rhs) const {
            return lhs->getClosure() < rhs;
        }
        bool operator()(const Closure& lhs, const ClosureState* rhs) const {
            return lhs < rhs->getClosure();
        }
        bool operator()(const Closure& lhs, const Closure& rhs) const {
            return lhs < rhs;
        }
    };
    
    ClosureAutomaton::ClosureAutomaton() :
    m_maxDeadlockDistance(0) {}

    const ClosureState* ClosureAutomaton::findState(const Closure& closure) const {
        ClosureState query(closure);
        StateSet::const_iterator it = getStates().find(&query);
        if (it == getStates().end())
            return NULL;
        return *it;
    }
    
    size_t ClosureAutomaton::getMaxDeadlockDistance() const {
        return m_maxDeadlockDistance;
    }
    
    void ClosureAutomaton::setMaxDeadlockDistnace(size_t maxDeadlockDistance) {
        m_maxDeadlockDistance = maxDeadlockDistance;
    }

    ClosureAutomaton::StateSet ClosureAutomaton::findUnreachableStates() const {
        StateSet unreachable;
        
        size_t count;
        do {
            count = unreachable.size();
            doFindUnreachableStates(unreachable);
        } while (count < unreachable.size());
        
        return unreachable;
    }

    void ClosureAutomaton::doFindUnreachableStates(StateSet& unreachable) const {
        const StateSet& states = getStates();
        StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClosureState* state = *it;
            if (state != getInitialState() &&
                unreachable.count(state) == 0 &&
                state->isPresetSubsetOfIgnoringLoops(unreachable))
                unreachable.insert(state);
        }
        
    }
}