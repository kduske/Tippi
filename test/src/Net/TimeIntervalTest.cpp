/*
 Copyright (C) 2013-2014 Kristian Duske
 
 This file is part of Tippi.
 
 Tippi is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Tippi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Tippi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "TimeInterval.h"
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
