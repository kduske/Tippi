//
//  TimeInterval.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#include "TimeInterval.h"
#include "Exceptions.h"

#include <limits>

namespace Tippi {
    namespace Interval {
        const size_t TimeInterval::Infinity = std::numeric_limits<size_t>::max();
        
        TimeInterval::TimeInterval(const size_t min, const size_t max) :
        m_min(min),
        m_max(max) {
            if (m_min > m_max) {
                NetException e;
                e << "Invalid interval bounds [" << min << ", " << max << "]";
                throw e;
            }
        }

        size_t TimeInterval::getMin() const {
            return m_min;
        }
        
        size_t TimeInterval::getMax() const {
            return m_max;
        }
    }
}
