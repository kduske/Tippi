//
//  Behavior.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__Behavior__
#define __Tippi__Behavior__

#include "BehaviorState.h"
#include "BehaviorEdge.h"

#include <cassert>

namespace Tippi {
    class NetState;
    
    class Behavior {
    private:
        BehaviorStateMap m_states;
        BehaviorState* m_initialState;
        BehaviorEdgeList m_edges;
    public:
        Behavior();
        ~Behavior();
        
        inline BehaviorState* behaviorState(const NetState& netState) const {
            BehaviorStateMap::const_iterator it = m_states.find(netState);
            if (it == m_states.end())
                return NULL;
            return it->second;
        }
        
        BehaviorState* createBehaviorState(const NetState& netState, bool final);
        
        inline BehaviorState* initialState() const {
            return m_initialState;
        }
        
        inline void setInitialState(BehaviorState* initialState) {
            assert(initialState != NULL);
            assert(m_initialState == NULL);
            m_initialState = initialState;
        }
        
        BehaviorEdge* connect(BehaviorState* source, BehaviorState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition);
    };
}

#endif /* defined(__Tippi__Behavior__) */
