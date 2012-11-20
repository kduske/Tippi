//
//  MPPState.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "MPPState.h"

namespace Tippi {
    MPPState::MPPState(DeadlockClass deadlockClass, const NetState& initialNetState, bool final) :
    m_deadlockClass(deadlockClass),
    m_initialNetState(initialNetState),
    m_final(final) {
        m_netStates.insert(initialNetState);
    }
}