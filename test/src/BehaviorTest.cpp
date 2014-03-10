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

#include "Exceptions.h"
#include "Behavior.h"
#include "Net/IntervalNet.h"
#include "Net/IntervalNetState.h"

namespace Tippi {
    namespace Behavior {
        TEST(BehaviorTest, createState) {
            Automaton behavior;
            
            const Interval::NetState netState(1, 1);
            const State* state = behavior.createState(netState);
            ASSERT_TRUE(state != NULL);
            ASSERT_EQ(state->getNetState(), netState);
        }
        
        TEST(BehaviorTest, createRedundantState) {
            Automaton behavior;
            
            const Interval::NetState netState(1, 1);
            behavior.createState(netState);
            ASSERT_THROW(behavior.createState(netState), AutomatonException);
        }
        
        TEST(BehaviorTest, findOrCreateState) {
            Automaton behavior;
            
            const Interval::NetState netState(1, 1);
            const State* state = behavior.createState(netState);
            const std::pair<State*, bool> result = behavior.findOrCreateState(netState);
            ASSERT_FALSE(result.second);
            ASSERT_EQ(state, result.first);
        }
        
        TEST(BehaviorTest, connectStates) {
            Automaton behavior;
            
            State* state1 = behavior.createState(Interval::NetState(Marking::createMarking(0, 0, 0),
                                                                    Marking::createMarking(0, 0)));
            State* state2 = behavior.createState(Interval::NetState(Marking::createMarking(1, 0, 0),
                                                                    Marking::createMarking(0, 0)));
            const Edge* edge = behavior.connectWithLabeledEdge(state1, state2, "test");
            ASSERT_EQ(state1, edge->getSource());
            ASSERT_EQ(state2, edge->getTarget());
            ASSERT_EQ(String("test"), edge->getLabel());
            ASSERT_TRUE(state1->isInPostset(state2));
            ASSERT_TRUE(state2->isInPreset(state1));
            
            ASSERT_EQ(edge, behavior.connectWithLabeledEdge(state1, state2, "test"));
            ASSERT_NE(edge, behavior.connectWithLabeledEdge(state1, state2, "test2"));
        }
        
        TEST(BehaviorTest, checkSimulation) {
            Automaton beh1;
            Automaton beh2;
            
            State* init1 = beh1.createState(Interval::NetState(Marking::createMarking(0, 0, 0),
                                                               Marking::createMarking(0, 0)));
            State* init2 = beh2.createState(Interval::NetState(Marking::createMarking(0, 0, 0),
                                                               Marking::createMarking(0, 0)));
            beh1.setInitialState(init1);
            beh2.setInitialState(init2);
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_a = beh2.createState(Interval::NetState(Marking::createMarking(0, 1, 0),
                                                                 Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2, init2_a, "a");
            ASSERT_FALSE(beh1.simulates(beh2));
            
            State* init1_a = beh1.createState(Interval::NetState(Marking::createMarking(0, 0, 1),
                                                                 Marking::createMarking(0, 0)));
            beh1.connectWithLabeledEdge(init1, init1_a, "a");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init1_b = beh1.createState(Interval::NetState(Marking::createMarking(0, 0, 2),
                                                                 Marking::createMarking(0, 0)));
            beh1.connectWithLabeledEdge(init1, init1_b, "b");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_b = beh2.createState(Interval::NetState(Marking::createMarking(0, 2, 0),
                                                                 Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2, init2_b, "b");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            beh1.connectWithLabeledEdge(init1_b, init1, "a");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_b_a = beh2.createState(Interval::NetState(Marking::createMarking(0, 3, 0),
                                                                   Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2_b, init2_b_a, "a");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_b_a_a = beh2.createState(Interval::NetState(Marking::createMarking(0, 4, 0),
                                                                     Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2_b_a, init2_b_a_a, "a");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_b_a_b = beh2.createState(Interval::NetState(Marking::createMarking(0, 5, 0),
                                                                     Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2_b_a, init2_b_a_b, "b");
            ASSERT_TRUE(beh1.simulates(beh2));
            
            State* init2_b_a_a_b = beh2.createState(Interval::NetState(Marking::createMarking(0, 6, 0),
                                                                       Marking::createMarking(0, 0)));
            beh2.connectWithLabeledEdge(init2_b_a_a, init2_b_a_a_b, "b");
            ASSERT_FALSE(beh1.simulates(beh2));
            
            /*
                  init1------|         init2
                   / \       |          / \
                a /   \ b    |       a /   \ b
                 /     \     |        /     \
             init1_a init1_b |    init2_a init2_b
                        |____|               |
                          a                  | a
                                             |
                                         init_2_b_a
                                            / \
                                         a /   \ b
                                          /     \
                                 init_2_b_a_a init2_b_a_b
                                       |
                                       | b
                                       |
                                init_2_b_a_a_b
             */
        }
    }
}
