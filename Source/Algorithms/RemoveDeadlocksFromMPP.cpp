//
//  RemoveDeadlocksFromMPP.cpp
//  Tippi
//
//  Created by Kristian Duske on 19.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "RemoveDeadlocksFromMPP.h"

#include "MPP.h"
#include "MPPState.h"

namespace Tippi {
    RemoveDeadlocksFromMPP::RemoveDeadlocksFromMPP(MPP& mpp) :
    m_mpp(mpp) {}
    
    void RemoveDeadlocksFromMPP::operator()() {
        MPPStateList states = m_mpp.states();
        MPPStateList::const_iterator stateIt, stateEnd;
        for (stateIt = states.begin(), stateEnd = states.end(); stateIt != stateEnd; ++stateIt) {
            MPPState& state = **stateIt;
            if (state.deadlockClass() == MPPState::Deadlock)
                m_mpp.deleteMPPState(state);
        }
    }
}