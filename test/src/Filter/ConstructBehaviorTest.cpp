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

#include "Filter/ConstructBehavior.h"
#include "Net/IntervalNet.h"
#include "Behavior.h"

namespace Tippi {
    static const size_t D = Interval::NetState::DisabledTransition;
    
    bool hasMarking(const Behavior::State* state,
                    const size_t A, const size_t B, const size_t C, const size_t a, const size_t b,
                    const size_t t1, const size_t t2, const size_t ta, const size_t tb) {
        return (state->getNetState().hasPlaceMarking(Marking::createMarking(A, B, C, a, b)) &&
                state->getNetState().hasTimeMarking(Marking::createMarking(t1, t2, ta, tb)));
    }
    
    bool hasOutgoingEdge(const Behavior::State* state, const Interval::Transition* transition) {
        const Behavior::State::OutgoingList& edges = state->getOutgoing();
        Behavior::State::OutgoingList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            const Behavior::Edge* edge = *it;
            if (edge->getLabel() == transition->getName())
                return true;
        }
        return false;
    }
    
    bool hasOutgoingTimeEdge(const Behavior::State* state) {
        const Behavior::State::OutgoingList& edges = state->getOutgoing();
        Behavior::State::OutgoingList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            const Behavior::Edge* edge = *it;
            if (edge->getLabel() == "1")
                return true;
        }
        return false;
    }
    
    TEST(ConstructBehaviorTest, simpleNet) {
        using Interval::Net;
        using Interval::Place;
        using Interval::Transition;
        using Interval::TimeInterval;
        using Behavior::State;
        using Behavior::Edge;
        
        ConstructBehavior::NetPtr net(new Net());
        
        Place* A = net->createPlace("A");
        Place* B = net->createPlace("B");
        Place* C = net->createPlace("C");
        
        Place* a = net->createPlace("a");
        a->setOutputPlace(true);
        Place* b = net->createPlace("b");
        b->setInputPlace(true);
        
        Transition* t1 = net->createTransition("t1", TimeInterval(1,2));
        Transition* t2 = net->createTransition("t2", TimeInterval(0,0));
        Transition* ta = net->createTransition("ta", TimeInterval(0, TimeInterval::Infinity));
        Transition* tb = net->createTransition("tb", TimeInterval(0, TimeInterval::Infinity));
        
        net->connect(A, t1);
        net->connect(t1, a);
        net->connect(t1, B);
        net->connect(B, t2);
        net->connect(b, t2);
        net->connect(t2, C);
        net->connect(a, ta);
        net->connect(tb, b);
        
        const Marking initialMarking = Marking::createMarking(1, 0, 0, 0, 0);
        const Marking   finalMarking = Marking::createMarking(0, 0, 1, 0, 0);
        
        net->setInitialMarking(initialMarking);
        net->addFinalMarking(finalMarking);
        
        ConstructBehavior::BehPtr beh = ConstructBehavior()(net);
        const State* i = beh->getInitialState();
        ASSERT_TRUE(i != NULL);
        ASSERT_TRUE(hasMarking(i,
                               1, 0, 0, 0, 0,
                               0,D,D,0));
        ASSERT_EQ(2u, i->getOutgoing().size());
        ASSERT_TRUE(hasOutgoingTimeEdge(i));
        ASSERT_TRUE(hasOutgoingEdge(i, tb));
        
        const State* i_tb = i->getSuccessor("tb");
        ASSERT_TRUE(hasMarking(i_tb,
                               1, 0, 0, 0, 1,
                               0, D, D, 0));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_tb));
        ASSERT_EQ(1u, i_tb->getOutgoing().size());
        
        const State* i_tb_1 = i_tb->getSuccessor("1");
        ASSERT_TRUE(hasMarking(i_tb_1,
                               1, 0, 0, 0, 1,
                               1, D, D, 0));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_tb_1));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_1, t1));
        ASSERT_EQ(2u, i_tb_1->getOutgoing().size());
        
        const State* i_tb_2 = i_tb_1->getSuccessor("1");
        ASSERT_TRUE(hasMarking(i_tb_2,
                               1, 0, 0, 0, 1,
                               2, D, D, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_2, t1));
        ASSERT_EQ(1u, i_tb_2->getOutgoing().size());
        
        const State* i_tb_12_t1 = i_tb_1->getSuccessor("t1");
        ASSERT_TRUE(hasMarking(i_tb_12_t1,
                               0, 1, 0, 1, 1,
                               D, 0, 0, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_12_t1, t2));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_12_t1, ta));
        ASSERT_EQ(2u, i_tb_12_t1->getOutgoing().size());
        ASSERT_EQ(i_tb_12_t1, i_tb_2->getSuccessor("t1"));
        
        const State* i_tb_12_t1_t2 = i_tb_12_t1->getSuccessor("t2");
        ASSERT_TRUE(hasMarking(i_tb_12_t1_t2,
                               0, 0, 1, 1, 0,
                               D, D, 0, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_12_t1_t2, ta));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_12_t1_t2, tb));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_tb_12_t1_t2));
        ASSERT_EQ(3u, i_tb_12_t1_t2->getOutgoing().size());
        ASSERT_EQ(i_tb_12_t1_t2, i_tb_12_t1_t2->getSuccessor("1"));
        
        const State* i_tb_12_t1_t2_tb = i_tb_12_t1_t2->getSuccessor("tb");
        ASSERT_TRUE(hasMarking(i_tb_12_t1_t2_tb,
                               0, 0, 1, 1, 1,
                               D, D, 0, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_tb_12_t1_t2_tb, ta));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_tb_12_t1_t2_tb));
        ASSERT_EQ(2u, i_tb_12_t1_t2_tb->getOutgoing().size());
        ASSERT_EQ(i_tb_12_t1_t2_tb, i_tb_12_t1_t2_tb->getSuccessor("1"));
        
        const State* i_1 = i->getSuccessor("1");
        ASSERT_TRUE(hasMarking(i_1,
                               1, 0, 0, 0, 0,
                               1, D, D, 0));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_1));
        ASSERT_TRUE(hasOutgoingEdge(i_1, t1));
        ASSERT_TRUE(hasOutgoingEdge(i_1, tb));
        ASSERT_EQ(i_tb_1, i_1->getSuccessor("tb"));
        
        const State* i_2 = i_1->getSuccessor("1");
        ASSERT_TRUE(hasMarking(i_2,
                               1, 0, 0, 0, 0,
                               2, D, D, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_2, tb));
        ASSERT_TRUE(hasOutgoingEdge(i_2, t1));
        ASSERT_EQ(2u, i_2->getOutgoing().size());
        ASSERT_EQ(i_tb_2, i_2->getSuccessor("tb"));
        
        const State* i_12_t1 = i_1->getSuccessor("t1");
        ASSERT_TRUE(hasMarking(i_12_t1,
                               0,1,0,1,0,
                               D, D, 0, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_12_t1, ta));
        ASSERT_TRUE(hasOutgoingEdge(i_12_t1, tb));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_12_t1));
        ASSERT_EQ(3u, i_12_t1->getOutgoing().size());
        ASSERT_EQ(i_12_t1, i_2->getSuccessor("t1"));
        ASSERT_EQ(i_12_t1, i_12_t1->getSuccessor("1"));
        
        ASSERT_EQ(i_tb_12_t1, i_12_t1->getSuccessor("tb"));
        
        const State* i_12_t1_ta = i_12_t1->getSuccessor("ta");
        ASSERT_TRUE(hasMarking(i_12_t1_ta,
                               0, 1, 0, 0, 0,
                               D, D, D, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_12_t1_ta, tb));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_12_t1_ta));
        ASSERT_EQ(2u, i_12_t1_ta->getOutgoing().size());
        ASSERT_EQ(i_12_t1_ta, i_12_t1_ta->getSuccessor("1"));
        
        const State* i_12_t1_ta_tb = i_12_t1_ta->getSuccessor("tb");
        ASSERT_TRUE(hasMarking(i_12_t1_ta_tb,
                               0, 1, 0, 0, 1,
                               D, 0, D, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_12_t1_ta_tb, t2));
        ASSERT_EQ(1u, i_12_t1_ta_tb->getOutgoing().size());
        ASSERT_EQ(i_12_t1_ta_tb, i_tb_12_t1->getSuccessor("ta"));
        
        const State* i_12_t1_ta_tb_t2 = i_12_t1_ta_tb->getSuccessor("t2");
        ASSERT_EQ(i_12_t1_ta_tb_t2, i_tb_12_t1_t2->getSuccessor("ta"));
        ASSERT_TRUE(hasMarking(i_12_t1_ta_tb_t2,
                               0, 0, 1, 0, 0,
                               D, D, D, 0));
        ASSERT_TRUE(hasOutgoingEdge(i_12_t1_ta_tb_t2, tb));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_12_t1_ta_tb_t2));
        ASSERT_EQ(2u, i_12_t1_ta_tb_t2->getOutgoing().size());
        ASSERT_EQ(i_12_t1_ta_tb_t2, i_12_t1_ta_tb_t2->getSuccessor("1"));
        
        const State* i_12_t1_ta_tb_t2_tb = i_12_t1_ta_tb_t2->getSuccessor("tb");
        ASSERT_EQ(i_12_t1_ta_tb_t2_tb, i_tb_12_t1_t2_tb->getSuccessor("ta"));
        ASSERT_TRUE(hasMarking(i_12_t1_ta_tb_t2_tb,
                               0, 0, 1, 0, 1,
                               D, D, D, 0));
        ASSERT_TRUE(hasOutgoingTimeEdge(i_12_t1_ta_tb_t2_tb));
        ASSERT_EQ(1u, i_12_t1_ta_tb_t2_tb->getOutgoing().size());
        ASSERT_EQ(i_12_t1_ta_tb_t2_tb, i_12_t1_ta_tb_t2_tb->getSuccessor("1"));
    }
}
