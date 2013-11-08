/*
 Copyright (C) 2013 Kristian Duske
 
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

        bool TimeInterval::operator==(const TimeInterval& rhs) const {
            return m_min == rhs.m_min && m_max == rhs.m_max;
        }

        size_t TimeInterval::getMin() const {
            return m_min;
        }
        
        size_t TimeInterval::getMax() const {
            return m_max;
        }

        bool TimeInterval::isBounded() const {
            return m_max != Infinity;
        }

        bool TimeInterval::contains(const size_t time) const {
            if (time < m_min)
                return false;
            if (m_max != Infinity && time > m_max)
                return false;
            return true;
        }
    }
}
