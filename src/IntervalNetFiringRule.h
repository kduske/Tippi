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

#ifndef __Tippi__IntervalNetFiringRule__
#define __Tippi__IntervalNetFiringRule__

#include "IntervalNet.h"
#include "IntervalNetState.h"

namespace Tippi {
    namespace Interval {
        class FiringRule {
        public:
            class Closure {
            private:
                NetState::Set m_states;
                bool m_containsLoop;
                bool m_containsBoundViolation;
            public:
                Closure();
                
                bool operator<(const Closure& rhs) const;
                bool operator==(const Closure& rhs) const;
                int compare(const Closure& rhs) const;

                bool isEmpty() const;
                bool containsState(const Interval::NetState& state) const;
                const NetState::Set& getStates() const;
                
                bool containsLoop() const;
                bool containsBoundViolation() const;

                bool addState(const NetState& state);
                void setContainsLoop();
                void setContainsBoundViolation();

                String asString(const String& markingSeparator, const String& stateSeparator) const;
            };
        private:
            const Net& m_net;
        public:
            FiringRule(const Net& net);
            
            Transition::List getFireableTransitions(const NetState& state) const;
            bool isFireable(const Transition* transition, const NetState& state) const;
            NetState fireTransition(const Transition* transition, const NetState& state) const;
            bool canMakeTimeStep(const NetState& state) const;
            NetState makeTimeStep(const NetState& state) const;
            Closure buildClosure(const NetState& state, const StringList& labels = StringList(1, "")) const;
            Closure buildClosure(const Closure& closure, const StringList& labels = StringList(1, "")) const;
            Closure buildClosure(const NetState::Set& states, const StringList& labels = StringList(1, "")) const;
        private:
            void updateTokens(const Transition* transition, NetState& state) const;
            void consumeTokens(const Transition* transition, NetState& state) const;
            void produceTokens(const Transition* transition, NetState& state) const;

            void updateSiblings(const Transition* transition, NetState& state) const;
            void updateSuccessors(const Transition* transition, NetState& state) const;
            void resetPostset(const Place* place, NetState& state) const;
            void enablePostset(const Place* place, NetState& state) const;
            void buildClosureRecurse(const NetState& state, const StringList& labels, Closure& closure) const;
        };
    }
}

#endif /* defined(__Tippi__IntervalNetFiringRule__) */
