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

#ifndef __Tippi__IntervalNetFiringRule__
#define __Tippi__IntervalNetFiringRule__

#include "Net/IntervalNet.h"
#include "Net/IntervalNetState.h"

namespace Tippi {
    namespace Interval {
        class FiringRule {
        private:
            const Net& m_net;
        public:
            FiringRule(const Net& net);
            
            Transition::List getFireableTransitions(const NetState& state) const;
            bool isFireable(const Transition* transition, const NetState& state) const;
            NetState fireTransition(const Transition* transition, const NetState& state) const;
            bool canMakeTimeStep(const NetState& state) const;
            NetState makeTimeStep(const NetState& state) const;
            std::pair<NetState::Set, bool> buildClosure(const NetState& state, const StringList& labels = StringList(1, "")) const;
            std::pair<NetState::Set, bool> buildClosure(const NetState::Set& states, const StringList& labels = StringList(1, "")) const;
        private:
            void updateTokens(const Transition* transition, NetState& state) const;
            void consumeTokens(const Transition* transition, NetState& state) const;
            void produceTokens(const Transition* transition, NetState& state) const;

            void updateSiblings(const Transition* transition, NetState& state) const;
            void updateSuccessors(const Transition* transition, NetState& state) const;
            void resetPostset(const Place* place, NetState& state) const;
            void enablePostset(const Place* place, NetState& state) const;
            bool buildClosureRecurse(const NetState& state, const StringList& labels, NetState::Set& states) const;
        };
    }
}

#endif /* defined(__Tippi__IntervalNetFiringRule__) */
