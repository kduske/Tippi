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

#include "IntervalNetState.h"

#include "Net/IntervalNet.h"

#include <cassert>
#include <limits>

namespace Tippi {
    namespace Interval {
        const size_t NetState::DisabledTransition = std::numeric_limits<size_t>::max();
        
        NetState::NetState(const size_t placeCount, const size_t transitionCount) :
        m_placeMarking(placeCount),
        m_timeMarking(transitionCount) {}

        NetState::NetState(const Marking& placeMarking, const Marking& timeMarking) :
        m_placeMarking(placeMarking),
        m_timeMarking(timeMarking) {}

        bool NetState::operator<(const NetState& rhs) const {
            return compare(rhs) < 0;
        }

        bool NetState::operator==(const NetState& rhs) const {
            return compare(rhs) == 0;
        }
        
        int NetState::compare(const NetState& rhs) const {
            const int placeResult = m_placeMarking.compare(rhs.m_placeMarking);
            if (placeResult < 0)
                return -1;
            if (placeResult > 0)
                return 1;
            return m_timeMarking.compare(rhs.m_timeMarking);
        }

        bool NetState::isPlaceEnabled(const Transition* transition) const {
            return m_timeMarking[transition] != DisabledTransition;
        }
        
        bool NetState::isTimeEnabled(const Transition* transition) const {
            return transition->getInterval().contains(m_timeMarking[transition]);
        }

        bool NetState::canMakeTimeStep(const size_t step, const Transition* transition) const {
            assert(isPlaceEnabled(transition));
            const size_t time = m_timeMarking[transition];
            const size_t max = transition->getInterval().getMax();
            assert(max == TimeInterval::Infinity || time <= max);
            return time + step <= max;
        }

        size_t NetState::getPlaceMarking(const Place* place) const {
            return m_placeMarking[place];
        }
        
        size_t NetState::getTimeMarking(const Transition* transition) const {
            return m_timeMarking[transition];
        }

        void NetState::updatePlaceMarking(const Place* place, const size_t marking) {
            m_placeMarking[place] = marking;
        }

        void NetState::resetTransition(const Transition* transition) {
            m_timeMarking[transition] = 0;
        }

        void NetState::disableTransition(const Transition* transition) {
            m_timeMarking[transition] = DisabledTransition;
        }

        String NetState::asString(const String separator) const {
            StringStream str;
            str << m_placeMarking.asString();
            str << separator;
            str << m_timeMarking.asString();
            return str.str();
        }
    }
}
