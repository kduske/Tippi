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

#ifndef __Tippi__ConstructBehavior__
#define __Tippi__ConstructBehavior__

#include "SharedPointer.h"
#include "StringUtils.h"

#include <iostream>

namespace Tippi {
    namespace Behavior {
        class Automaton;
        class State;
    }
    
    namespace Interval {
        class FiringRule;
        class Net;
        class NetState;
    }
    
    struct ConstructBehavior {
    private:
        bool m_createBoundViolationState;
    public:
        typedef std::tr1::shared_ptr<Interval::Net> NetPtr;
        typedef std::tr1::shared_ptr<Behavior::Automaton> BehPtr;

        ConstructBehavior(bool createBoundViolationState);
        BehPtr operator()(const NetPtr net) const;
    private:
        void handleState(const NetPtr net, const Interval::FiringRule& rule, Behavior::State* state, Behavior::Automaton* automaton) const;
        void handleNetState(const NetPtr net, const Interval::FiringRule& rule, Behavior::State* state, const Interval::NetState& succNetState, const String& edgeLabel, Behavior::Automaton* automaton) const;
    };
}

#endif /* defined(__Tippi__ConstructBehavior__) */
