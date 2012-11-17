//
//  MPPEdge.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "MPPEdge.h"

namespace Tippi {
    MPPEdge::MPPEdge(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition) :
    GraphEdge(source, target),
    m_minTime(minTime),
    m_maxTime(maxTime),
    m_transition(transition) {}
}