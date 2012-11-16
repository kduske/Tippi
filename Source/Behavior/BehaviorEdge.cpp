//
//  BehaviorEdge.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "BehaviorEdge.h"

namespace Tippi {
    BehaviorEdge::BehaviorEdge(BehaviorState* source, BehaviorState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition) :
    GraphEdge(source, target),
    m_minTime(minTime),
    m_maxTime(maxTime),
    m_transition(transition) {}
}