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

#include "CollectionUtils.h"
#include "Net/IntervalNet.h"
#include "Net/IntervalNetFiringRule.h"
#include "Net/IntervalNetParser.h"
#include "Net/IntervalNetState.h"
#include "Net/Marking.h"

#include <limits>

namespace Tippi {
    namespace Interval {
        static const size_t _ = std::numeric_limits<size_t>::max(); // Attention! Must be adapted if NetState::DisabledTransition changes!
        
        struct LabelingFunction {
            String operator()(const Transition* transition) const {
                return "";
            }
        };
        
        TEST(IntervalNetFiringRuleTest, buildClosure) {
            Net net;
            Place* A = net.createPlace("A");
            Place* B = net.createPlace("B");
            Place* C = net.createPlace("C");
            Place* D = net.createPlace("D");
            Transition* t1 = net.createTransition("t1", TimeInterval(1,2));
            Transition* t2 = net.createTransition("t2", TimeInterval(0,2));
            Transition* t3 = net.createTransition("t3", TimeInterval(0,2));
            Transition* t4 = net.createTransition("t4", TimeInterval(3,4));
            
            net.connect(A, t1);
            net.connect(A, t2);
            net.connect(t1, B);
            net.connect(t1, C);
            net.connect(t2, D);
            net.connect(B, t3);
            net.connect(C, t4);
            net.connect(t3, A);
            net.connect(t4, D);
            
            net.setInitialMarking(Marking::createMarking(1, 0, 0, 0));
            net.setTransitionLabels(LabelingFunction());

            const NetState initial = NetState::createInitialState(net);
            const FiringRule rule(net);
            
            const std::pair<NetState::Set, bool> iResult = rule.buildClosure(initial);
            const NetState::Set& iCl = iResult.first;
            ASSERT_TRUE(iResult.second);
            ASSERT_EQ(2u, iCl.size());
            ASSERT_TRUE(iCl.count(initial) == 1);
            ASSERT_TRUE(iCl.count(rule.fireTransition(t2, initial)) == 1);
            
            const NetState state1(Marking::createMarking(1, 0, 0, 0),
                                  Marking::createMarking(2, 2, _, _));
            const std::pair<NetState::Set, bool> state1Result = rule.buildClosure(state1);
            const NetState::Set state1Cl = state1Result.first;
            ASSERT_TRUE(state1Result.second);
            ASSERT_EQ(5u, state1Cl.size());
            ASSERT_TRUE(state1Cl.count(state1) == 1);
            ASSERT_TRUE(state1Cl.count(NetState(Marking::createMarking(0, 0, 0, 1),
                                               Marking::createMarking(_, _, _, _))) == 1);
            ASSERT_TRUE(state1Cl.count(NetState(Marking::createMarking(0, 1, 1, 0),
                                               Marking::createMarking(_, _, 0, 0))) == 1);
            ASSERT_TRUE(state1Cl.count(NetState(Marking::createMarking(1, 0, 1, 0),
                                               Marking::createMarking(0, 0, _, 0))) == 1);
            ASSERT_TRUE(state1Cl.count(NetState(Marking::createMarking(0, 0, 1, 1),
                                               Marking::createMarking(_, _, _, 0))) == 1);
            
            const NetState state2(Marking::createMarking(1, 0, 1, 0),
                                  Marking::createMarking(2, 2, _, 3));
            const std::pair<NetState::Set, bool> state2Result = rule.buildClosure(state2);
            const NetState::Set state2Cl = state2Result.first;
            ASSERT_FALSE(state2Result.second);
        }
    }
}
