//
//  MPP.h
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__MPP__
#define __Tippi__MPP__

#include "MPPState.h"
#include "MPPEdge.h"

#include <cassert>

namespace Tippi {
    class BehaviorState;
    class Transition;
    
    class MPP {
    private:
        MPPStateMap m_states;
        MPPState* m_initialState;
        MPPEdgeList m_edges;
    public:
        MPP();
        ~MPP();
        
        inline MPPState* initialState() const {
            return m_initialState;
        }
        
        inline void setInitialState(MPPState* initialState) {
            assert(m_initialState == NULL);
            assert(initialState != NULL);
            m_initialState = initialState;
        }
        
        inline MPPState* mppState(const NetState& netState) const {
            MPPStateMap::const_iterator it = m_states.find(netState);
            if (it == m_states.end())
                return NULL;
            return it->second;
        }
        
        MPPState* createMPPState(MPPState::DeadlockClass deadlockClass, const BehaviorState& behaviorState);
        
        MPPEdge* connect(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition);
    };
}

#endif /* defined(__Tippi__MPP__) */
