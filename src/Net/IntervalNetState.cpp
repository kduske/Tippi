//
//  IntervalNetState.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#include "IntervalNetState.h"

namespace Tippi {
    namespace Interval {
        NetState::NetState(const size_t placeCount, const size_t transitionCount) :
        m_placeMarking(placeCount),
        m_timeMarking(transitionCount) {}
    }
}
