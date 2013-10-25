//
//  IntervalNetState.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#ifndef __Tippi__IntervalNetState__
#define __Tippi__IntervalNetState__

#include "Net/Marking.h"

namespace Tippi {
    namespace Interval {
        class NetState {
        private:
            Marking m_placeMarking;
            Marking m_timeMarking;
        public:
            NetState(const size_t placeCount, const size_t transitionCount);
        };
    }
}

#endif /* defined(__Tippi__IntervalNetState__) */
