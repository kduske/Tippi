//
//  MPP.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "MPP.h"

#include "BehaviorState.h"

namespace Tippi {
    MPP::MPP() :
    m_initialState(NULL) {}
    
    MPP::~MPP() {
        MPPStateMap::iterator it, end;
        for (it = m_states.begin(), end = m_states.end(); it != end; ++it)
            delete it->second;
        m_states.clear();
        m_initialState = NULL;
        while (!m_edges.empty()) delete m_edges.back(), m_edges.pop_back();
    }

    MPPState* MPP::createMPPState(MPPState::DeadlockClass deadlockClass, const BehaviorState& behaviorState) {
        MPPState* mppState = new MPPState(deadlockClass, behaviorState.netState(), behaviorState.isFinal());
        MPPStateMapInsertResult result = m_states.insert(MPPStateMapEntry(behaviorState.netState(), mppState));
        assert(result.second);
        return mppState;
    }

    MPPEdge* MPP::connect(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition) {
        MPPEdge* mppEdge = new MPPEdge(source, target, minTime, maxTime, transition);
        source->addOutgoingEdge(mppEdge);
        target->addIncomingEdge(mppEdge);
        m_edges.push_back(mppEdge);
        return mppEdge;
    }
}
