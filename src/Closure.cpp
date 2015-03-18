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
    m_safety(Safety_Unknown),
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

    bool ClosureState::isSafetyKnown() const {
        return isEmpty() || m_safety != Safety_Unknown;
    }
    
    bool ClosureState::isSafe() const {
        if (isEmpty())
            return true;
        
        assert(isSafetyKnown());
        return m_safety == Safety_Safe;
    }

    void ClosureState::setSafe(const bool safe) {
        assert(!isEmpty());
        assert(!isSafetyKnown());
        m_safety = safe ? Safety_Safe : Safety_Unsafe;
    }

    bool ClosureState::isReachable() const {
        return m_reachable;
    }
    
    void ClosureState::setReachable(bool reachable) {
        m_reachable = reachable;
    }

    bool ClosureState::isStable() const {
        const Outgoing::List& outgoing = getOutgoing();
        Outgoing::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const Outgoing* edge = *it;
            const ClosureState* successor = edge->getTarget();
            if (!successor->isEmpty()) {
                if (edge->isServiceAction() || (edge->isTimeAction() && !edge->isLoop()))
                    return false;
            }
        }
        return true;
    }

    bool ClosureState::isDeadlock() const {
        const Outgoing::List& outgoing = getOutgoing();
        Outgoing::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const Outgoing* edge = *it;
            if (!edge->getTarget()->isEmpty() && !edge->isLoop())
                return false;
        }
        return true;
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
    m_boundViolationState(NULL) {}
    
    ClosureState* ClosureAutomaton::boundViolationState(const Closure& closure) {
        assert(closure.containsBoundViolation());
        
        if (m_boundViolationState == NULL) {
            m_boundViolationState = createState(closure);
        } else {
            const Closure& oldClosure = m_boundViolationState->getClosure();
            
            Closure newClosure;
            newClosure.addStates(oldClosure.getStates());
            newClosure.addStates(closure.getStates());
            newClosure.setContainsBoundViolation();

            ClosureState* newState = createState(newClosure);
            replaceState(m_boundViolationState, newState);
            m_boundViolationState = newState;
        }
        
        return m_boundViolationState;
    }

    const ClosureState* ClosureAutomaton::findState(const Closure& closure) const {
        ClosureState query(closure);
        StateSet::const_iterator it = getStates().find(&query);
        if (it == getStates().end())
            return NULL;
        return *it;
    }

    ClosureAutomaton::StateSet ClosureAutomaton::findUnsafeStates() const {
        StateSet result;
        const StateSet& states = getStates();
        StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            ClosureState* state = *it;
            if (state->isSafetyKnown() && !state->isSafe())
                result.insert(state);
        }
        return result;
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