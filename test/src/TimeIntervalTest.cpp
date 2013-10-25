//
//  TimeIntervalTest.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#include <gtest/gtest.h>

#include "Net/TimeInterval.h"
#include "Exceptions.h"

namespace Tippi {
    namespace Interval {
        TEST(TimeIntervalTest, createDefaultInterval) {
            const TimeInterval i;
            ASSERT_EQ(0u, i.getMin());
            ASSERT_EQ(TimeInterval::Infinity, i.getMax());
        }
        
        TEST(TimeIntervalTest, createZeroLengthInterval) {
            ASSERT_NO_THROW(TimeInterval(0,0));
            ASSERT_NO_THROW(TimeInterval(0,1));
            ASSERT_NO_THROW(TimeInterval(2,20));
        }
        
        TEST(TimeIntervalTest, createInvalidInterval) {
            ASSERT_THROW(TimeInterval(1, 0), NetException);
            ASSERT_THROW(TimeInterval(5, 4), NetException);
        }
    }
}
