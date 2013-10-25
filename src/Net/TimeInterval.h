//
//  TimeInterval.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#ifndef __Tippi__TimeInterval__
#define __Tippi__TimeInterval__

#include <iostream>

namespace Tippi {
    namespace Interval {
        class TimeInterval {
        public:
            static const size_t Infinity;
        private:
            size_t m_min;
            size_t m_max;
        public:
            TimeInterval(const size_t min = 0, const size_t max = Infinity);
            
            size_t getMin() const;
            size_t getMax() const;
        };
    }
}

#endif /* defined(__Tippi__TimeInterval__) */
