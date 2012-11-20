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
        
        inline MPPStateList states() const {
            MPPStateList result;
            MPPStateMap::const_iterator stateIt, stateEnd;
            for (stateIt = m_states.begin(), stateEnd = m_states.end(); stateIt != stateEnd; ++stateIt) {
                MPPState* state = stateIt->second;
                result.push_back(state);
            }
            return result;
        }
        
        MPPState* createMPPState(MPPState::DeadlockClass deadlockClass, const BehaviorState& behaviorState);
        void deleteMPPState(MPPState& state);
        
        MPPEdge* connect(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition);
    };
}

#endif /* defined(__Tippi__MPP__) */
