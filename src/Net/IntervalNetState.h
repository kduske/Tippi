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
