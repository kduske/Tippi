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

#include <gtest/gtest.h>

#include "Exceptions.h"
#include "Behavior.h"
#include "Net/IntervalNet.h"
#include "Net/IntervalNetState.h"

namespace Tippi {
    namespace Behavior {
        TEST(AutomatonTest, createState) {
            Automaton behavior;

            const Interval::NetState netState(1, 1);
            const State* state = behavior.createState(netState);
            ASSERT_TRUE(state != NULL);
            ASSERT_EQ(state->getNetState(), netState);
        }

        TEST(AutomatonTest, createRedundantState) {
            Automaton behavior;
            
            const Interval::NetState netState(1, 1);
            behavior.createState(netState);
            ASSERT_THROW(behavior.createState(netState), AutomatonException);
        }
        
        TEST(AutomatonTest, findOrCreateState) {
            Automaton behavior;
            
            const Interval::NetState netState(1, 1);
            const State* state = behavior.createState(netState);
            const std::pair<State*, bool> result = behavior.findOrCreateState(netState);
            ASSERT_FALSE(result.second);
            ASSERT_EQ(state, result.first);
        }
        
        TEST(AutomatonTest, connectStates) {
            Automaton behavior;
            
            State* state1 = behavior.createState(Interval::NetState(Marking::createMarking(3, 0, 0, 0),
                                                                    Marking::createMarking(2, 0, 0)));
            State* state2 = behavior.createState(Interval::NetState(Marking::createMarking(3, 1, 0, 0),
                                                                    Marking::createMarking(2, 0, 0)));
            const Edge* edge = behavior.connect(state1, state2, "test");
            ASSERT_EQ(state1, edge->getSource());
            ASSERT_EQ(state2, edge->getTarget());
            ASSERT_EQ(String("test"), edge->getLabel());
            ASSERT_TRUE(state1->isInPostset(state2));
            ASSERT_TRUE(state2->isInPreset(state1));
            
            ASSERT_THROW(behavior.connect(state1, state2, "test"), AutomatonException);
            ASSERT_NO_THROW(behavior.connect(state1, state2, "test2"));
        }
    }
}
