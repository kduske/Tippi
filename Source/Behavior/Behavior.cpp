//
//  Behavior.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "Behavior.h"

namespace Tippi {
    Behavior::Behavior() :
    m_initialState(NULL) {}

    Behavior::~Behavior() {
        BehaviorStateMap::iterator it, end;
        for (it = m_states.begin(), end = m_states.end(); it != end; ++it)
            delete it->second;
        m_states.clear();
        m_initialState = NULL;
        while (!m_edges.empty()) delete m_edges.back(), m_edges.pop_back();
    }

    BehaviorState* Behavior::createBehaviorState(const NetState& netState, bool final) {
        BehaviorState* newState = new BehaviorState(netState, final);
        BehaviorStateMapInsertResult result = m_states.insert(BehaviorStateMapEntry(netState, newState));
        assert(result.second);
        return newState;
    }

    BehaviorEdge* Behavior::connect(BehaviorState* source, BehaviorState* target, unsigned int time, const Transition& transition) {
        BehaviorEdge* newEdge = new BehaviorEdge(source, target, time, transition);
        source->addOutgoingEdge(newEdge);
        target->addIncomingEdge(newEdge);
        m_edges.push_back(newEdge);
        return newEdge;
    }
}