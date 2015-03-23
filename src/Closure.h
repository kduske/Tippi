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
#include "IntervalNetFiringRule.h"
#include "IntervalNetState.h"

namespace Tippi {
    typedef Interval::FiringRule::Closure Closure;
    class ClosureState;
    
    class ClosureEdge : public AutomatonEdge<ClosureState> {
    public:
        typedef std::vector<ClosureEdge*> List;
        
        typedef enum {
            EdgeType_InputSend,
            EdgeType_InputRead,
            EdgeType_OutputSend,
            EdgeType_OutputRead,
            EdgeType_Time
        } EdgeType;
    private:
        EdgeType m_type;
    public:
        ClosureEdge(ClosureState* source, ClosureState* target, const String& label, EdgeType type);
        
        EdgeType getType() const;
        bool isServiceAction() const;
        bool isPartnerAction() const;
        bool isTimeAction() const;
        
        String asString() const;
    };
    
    class ClosureState : public AutomatonState<ClosureState, ClosureEdge> {
    private:
        typedef enum {
            Safety_Safe,
            Safety_Unsafe,
            Safety_Unknown
        } Safety;
    public:
        typedef Closure Key;
        struct KeyCmp {
            int operator() (const Key& lhs, const Key& rhs) const;
        };
    private:
        Closure m_closure;
        Safety m_safety;
        bool m_reachable;
    public:
        ClosureState(const Closure& closure);
        static const Key& getKey(const ClosureState* state);
        
        const Closure& getClosure() const;
        bool isEmpty() const;
        bool isBoundViolation() const;

        bool isSafetyKnown() const;
        bool isSafe() const;
        void setSafe(bool safe);
        
        bool isReachable() const;
        void setReachable(bool reachable);

        bool isStable() const;
        bool isDeadlock() const;
        
        String asString() const;
        String asString(const String& markingSeparator, const String& stateSeparator) const;
    };
    
    class ClosureAutomaton : public Automaton<ClosureState, ClosureEdge> {
    private:
        ClosureState* m_boundViolationState;
    public:
        typedef std::tr1::shared_ptr<ClosureAutomaton> Ptr;
    public:
        ClosureAutomaton();
        
        ClosureState* boundViolationState(const Closure& closure);
        
        const ClosureState* findState(const Closure& closure) const;

        StateSet findUnsafeStates() const;
        StateSet findUnreachableStates() const;
    private:
        void doFindUnreachableStates(StateSet& unreachable) const;
    };
}

#endif /* defined(__Tippi__Behavior__) */
