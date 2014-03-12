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

#include <set>
#include <vector>

namespace Tippi {
    class BehaviorState;
    
    class BehaviorEdge : public AutomatonEdge<BehaviorState> {
    public:
        typedef std::vector<BehaviorEdge*> List;
        typedef std::set<BehaviorEdge*, Utils::UniCmp<BehaviorEdge> > Set;
    public:
        BehaviorEdge(BehaviorState* source, BehaviorState* target, const String& label, bool tauEdge);
        
        bool operator<(const BehaviorEdge& rhs) const;
        bool operator<(const BehaviorEdge* rhs) const;
    };
    
    class BehaviorState : public AutomatonState<BehaviorEdge> {
    public:
        typedef std::set<BehaviorState*, Utils::UniCmp<BehaviorState> > Set;
    private:
        Interval::NetState m_netState;
        bool m_boundViolation;
    public:
        BehaviorState(const String& name, const Interval::NetState& netState);
        BehaviorState(const String& name);
        
        bool operator<(const BehaviorState& rhs) const;
        bool operator<(const BehaviorState* rhs) const;
        int compare(const BehaviorState& rhs) const;
        
        const Interval::NetState& getNetState() const;
        bool isBoundViolation() const;
        
        String asString(const String separator = " ") const;
    };
    
    class Behavior : public Automaton<BehaviorState, BehaviorEdge> {
    public:
        typedef std::tr1::shared_ptr<Behavior> Ptr;
    private:
        unsigned long m_stateIndex;
        State* m_boundViolationState;
    public:
        Behavior();
        
        BehaviorState* createState(const Interval::NetState& netState);
        std::pair<BehaviorState*, bool> findOrCreateState(const Interval::NetState& netState);
        BehaviorState* findOrCreateBoundViolationState();
    private:
        String makeStateName();
    };
}

#endif /* defined(__Tippi__Behavior__) */
