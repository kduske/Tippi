//
//  BehaviorState.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "BehaviorState.h"

namespace Tippi {
    BehaviorState::BehaviorState(const NetState& netState, bool final) :
    GraphNode(),
    m_netState(netState),
    m_final(final) {}
    
    BehaviorState::~BehaviorState() {}
}