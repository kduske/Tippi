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

#ifndef __Tippi__Behavior__
#define __Tippi__Behavior__

#include "SharedPointer.h"
#include "StringUtils.h"
#include "Automaton.h"
#include "IntervalNetState.h"

namespace Tippi {
    class ClosureState;
    
    class ClosureEdge : public AutomatonEdge<ClosureState> {
    public:
        typedef std::vector<ClosureEdge*> List;
        
        typedef enum {
            InputSend,
            InputRead,
            OutputSend,
            OutputRead,
            Time
        } Type;
    private:
        Type m_type;
    public:
        ClosureEdge(ClosureState* source, ClosureState* target, const String& label, Type type);
        
        Type getType() const;
        bool isServiceAction() const;
        bool isPartnerAction() const;
        bool isTimeAction() const;
        
        String asString() const;
    };
    
    class Closure {
    private:
        Interval::NetState::Set m_netStates;
    public:
        Closure();
        Closure(const Interval::NetState::Set& netStates);
        
        bool operator<(const Closure& rhs) const;
        bool operator==(const Closure& rhs) const;
        int compare(const Closure& rhs) const;
        
        bool contains(const Interval::NetState& state) const;
        const Interval::NetState::Set& getStates() const;
        bool isEmpty() const;
        String asString(const String& markingSeparator, const String& stateSeparator) const;
    };
    
    class ClosureState : public AutomatonState<ClosureEdge> {
    public:
        typedef Closure Key;
        struct KeyCmp {
            int operator() (const Key& lhs, const Key& rhs) const;
        };
    private:
        Closure m_closure;
        size_t m_deadlockDistance;
        bool m_reachable;
    public:
        ClosureState(const Closure& closure);
        static const Key& getKey(const ClosureState* state);
        
        const Closure& getClosure() const;
        bool isEmpty() const;

        bool isDeadlock() const;
        size_t getDeadlockDistance() const;
        void setDeadlockDistance(size_t deadlockDistance);
        
        bool isReachable() const;
        void setReachable(bool reachable);

        String asString() const;
        String asString(const String& markingSeparator, const String& stateSeparator) const;
    };
    
    class ClosureAutomaton : public Automaton<ClosureState, ClosureEdge> {
    public:
        typedef std::tr1::shared_ptr<ClosureAutomaton> Ptr;
    private:
        size_t m_maxDeadlockDistance;
    public:
        ClosureAutomaton();
        
        const ClosureState* findState(const Closure& closure) const;

        size_t getMaxDeadlockDistance() const;
        void setMaxDeadlockDistnace(size_t maxDeadlockDistance);
        
        StateSet findUnreachableStates() const;
    private:
        void doFindUnreachableStates(StateSet& unreachable) const;
    };
}

#endif /* defined(__Tippi__Behavior__) */
