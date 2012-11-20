//
//  MPP.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "MPP.h"

#include "BehaviorState.h"

#include <algorithm>

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
    
    void MPP::deleteMPPState(MPPState& state) {
        MPPStateMap::iterator it = m_states.find(state.initialNetState());
        assert(it != m_states.end());
        assert(it->second == &state);
        
        const MPPState::IncomingEdgeList& incomingEdges = state.incomingEdges();
        MPPState::IncomingEdgeList::const_iterator inEdgeIt, inEdgeEnd;
        for (inEdgeIt = incomingEdges.begin(), inEdgeEnd = incomingEdges.end(); inEdgeIt != inEdgeEnd; ++inEdgeIt) {
            MPPEdge* edge = *inEdgeIt;
            edge->source()->removeOutgoingEdge(edge);
            m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());
            delete edge;
        }
        
        const MPPState::OutgoingEdgeList& outgoingEdges = state.outgoingEdges();
        MPPState::OutgoingEdgeList::const_iterator outEdgeIt, outEdgeEnd;
        for (outEdgeIt = outgoingEdges.begin(), outEdgeEnd = outgoingEdges.end(); outEdgeIt != outEdgeEnd; ++outEdgeIt) {
            MPPEdge* edge = *outEdgeIt;
            edge->target()->removeIncomingEdge(edge);
            m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());
            delete edge;
        }
        
        m_states.erase(it);
        delete &state;
    }

    MPPEdge* MPP::connect(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition) {
        MPPEdge* mppEdge = new MPPEdge(source, target, minTime, maxTime, transition);
        source->addOutgoingEdge(mppEdge);
        target->addIncomingEdge(mppEdge);
        m_edges.push_back(mppEdge);
        return mppEdge;
    }
}
