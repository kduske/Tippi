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

#include "Automaton.h"
#include "StringUtils.h"
#include "SharedPointer.h"
#include "GraphEdge.h"
#include "GraphNode.h"
#include "IntervalNetState.h"

#include <vector>

namespace Tippi {
    class BehaviorState;
    
    class BehaviorEdge : public AutomatonEdge<BehaviorState> {
    public:
        typedef std::vector<BehaviorEdge*> List;
    public:
        BehaviorEdge(BehaviorState* source, BehaviorState* target, const String& label);
        BehaviorEdge(BehaviorState* source, BehaviorState* target);
    };
    
    class BehaviorState : public AutomatonState<BehaviorEdge> {
    public:
        struct Key {
            const Interval::NetState& netState;
            const bool boundViolation;
            Key(const Interval::NetState& i_netState, bool i_boundViolation = false);
            Key(const BehaviorState* state);
        };
        struct KeyCmp {
            int operator() (const Key& lhs, const Key& rhs) const;
        };
    private:
        Interval::NetState m_netState;
        bool m_boundViolation;
    public:
        BehaviorState(const Interval::NetState& netState);
        BehaviorState();
        static const Key getKey(const BehaviorState* state);
        
        const Interval::NetState& getNetState() const;
        bool isBoundViolation() const;
        
        String asString(const String separator = " ") const;
    };
    
    class Behavior : public Automaton<BehaviorState, BehaviorEdge> {
    public:
        typedef std::tr1::shared_ptr<Behavior> Ptr;
    private:
        BehaviorState* m_boundViolationState;
    public:
        Behavior();
        BehaviorState* findOrCreateBoundViolationState();
    };
}

#endif /* defined(__Tippi__Behavior__) */
