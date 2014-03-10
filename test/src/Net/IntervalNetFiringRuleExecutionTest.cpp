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

namespace Tippi {
    namespace Interval {
        class IntervalNetFiringRuleExecutionTest : public ::testing::Test {
        protected:
            Net* net;
            Transition* t1;
            Transition* t2;
            Transition* t3;
            Transition* ta;
            Transition* tb;
            const size_t DT;
        public:
            IntervalNetFiringRuleExecutionTest() :
            net(NULL),
            t1(NULL),
            t2(NULL),
            t3(NULL),
            ta(NULL),
            tb(NULL),
            DT(NetState::DisabledTransition) {
                const String str("TIMENET\n"
                                 "PLACE\n"
                                 "SAFE A,B,C,D,a,b;\n"
                                 "MARKING A:1;\n"
                                 "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE B:1,a:1;\n"
                                 "TRANSITION t2 TIME 3,4; CONSUME B:1; PRODUCE C:1;\n"
                                 "TRANSITION t3 TIME 1,2; CONSUME B:1,b:1; PRODUCE D:1;\n"
                                 "TRANSITION ta TIME 0,*; CONSUME a:1;\n"
                                 "TRANSITION tb TIME 0,*; PRODUCE b:1;\n"
                                 "FINALMARKING D:1;");
                
                NetParser parser(str);
                net = parser.parse();
                t1 = net->findTransition("t1");
                t2 = net->findTransition("t2");
                t3 = net->findTransition("t3");
                ta = net->findTransition("ta");
                tb = net->findTransition("tb");
            }
            
            ~IntervalNetFiringRuleExecutionTest() {
                delete net;
                net = NULL;
            }

            
            void assertPlaceMarking(const NetState& state, const size_t A, const size_t B, const size_t C, const size_t D, const size_t a, const size_t b) {
                ASSERT_EQ(A, state.getPlaceMarking(net->findPlace("A")));
                ASSERT_EQ(B, state.getPlaceMarking(net->findPlace("B")));
                ASSERT_EQ(C, state.getPlaceMarking(net->findPlace("C")));
                ASSERT_EQ(D, state.getPlaceMarking(net->findPlace("D")));
                ASSERT_EQ(a, state.getPlaceMarking(net->findPlace("a")));
                ASSERT_EQ(b, state.getPlaceMarking(net->findPlace("b")));
            }
            
            void assertTimeMarking(const NetState& state, const size_t t1m, const size_t t2m, const size_t t3m, const size_t tam, const size_t tbm) {
                ASSERT_EQ(t1m, state.getTimeMarking(t1));
                ASSERT_EQ(t2m, state.getTimeMarking(t2));
                ASSERT_EQ(t3m, state.getTimeMarking(t3));
                ASSERT_EQ(tam, state.getTimeMarking(ta));
                ASSERT_EQ(tbm, state.getTimeMarking(tb));
            }
            
            void assertFireableTransitions(const Transition::List& transitions, const bool t1e, const bool t2e, const bool t3e, const bool tae, const bool tbe) {
                size_t count = 0;
                if (t1e)
                    ++count;
                if (t2e)
                    ++count;
                if (t3e)
                    ++count;
                if (tae)
                    ++count;
                if (tbe)
                    ++count;
                
                ASSERT_EQ(count, transitions.size());
                if (t1e)
                    ASSERT_TRUE(VectorUtils::contains(transitions, t1));
                if (t2e)
                    ASSERT_TRUE(VectorUtils::contains(transitions, t2));
                if (t3e)
                    ASSERT_TRUE(VectorUtils::contains(transitions, t3));
                if (tae)
                    ASSERT_TRUE(VectorUtils::contains(transitions, ta));
                if (tbe)
                    ASSERT_TRUE(VectorUtils::contains(transitions, tb));
            }
        };
        
        TEST_F(IntervalNetFiringRuleExecutionTest, testFullNetExecution) {
            const FiringRule rule(*net);
            
            const NetState init = NetState::createInitialState(*net);
            assertPlaceMarking(init, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init, 0, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init));
            
            const NetState i_1 = rule.makeTimeStep(init);
            assertPlaceMarking(i_1, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(i_1, 1, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_1), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_1));

            const NetState i_2 = rule.makeTimeStep(i_1);
            assertPlaceMarking(i_2, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(i_2, 2, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_2), true, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_2));

            const NetState i_3 = rule.makeTimeStep(i_2);
            assertPlaceMarking(i_3, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(i_3, 3, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_3), true, false, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_3));
         
            const NetState i_23_t1 = rule.fireTransition(t1, i_2);
            assertPlaceMarking(i_23_t1, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(i_23_t1, DT, 0, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1));

            // Fire external transition tb:
            const NetState i_tb = rule.fireTransition(tb, init);
            assertPlaceMarking(i_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(i_tb, 0, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_tb));
            
            const NetState i_1_tb = rule.fireTransition(tb, i_1);
            assertPlaceMarking(i_1_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(i_1_tb, 1, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_1_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_1_tb));
            ASSERT_EQ(i_1_tb, rule.makeTimeStep(i_tb));
            
            const NetState i_2_tb = rule.fireTransition(tb, i_2);
            assertPlaceMarking(i_2_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(i_2_tb, 2, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_2_tb), true, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_2_tb));
            ASSERT_EQ(i_2_tb, rule.makeTimeStep(i_1_tb));

            const NetState i_3_tb = rule.fireTransition(tb, i_3);
            assertPlaceMarking(i_3_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(i_3_tb, 3, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_3_tb), true, false, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_3_tb));
            ASSERT_EQ(i_3_tb, rule.makeTimeStep(i_2_tb));

            const NetState i_23_t1_tb = rule.fireTransition(tb, i_23_t1);
            assertPlaceMarking(i_23_t1_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_tb, DT, 0, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_tb));
            ASSERT_EQ(i_23_t1_tb, rule.fireTransition(t1, i_2_tb));
            ASSERT_EQ(i_23_t1_tb, rule.fireTransition(t1, i_3_tb));
            
            const NetState i_23_t1_tb_1 = rule.makeTimeStep(i_23_t1_tb);
            assertPlaceMarking(i_23_t1_tb_1, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_tb_1, DT, 1, 1, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_tb_1), false, false, true, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_tb_1));

            const NetState i_23_t1_tb_2 = rule.makeTimeStep(i_23_t1_tb_1);
            assertPlaceMarking(i_23_t1_tb_2, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_tb_2, DT, 2, 2, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_tb_2), false, false, true, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_tb_2));

            const NetState i_23_t1_tb_12_t3 = rule.fireTransition(t3, i_23_t1_tb_1);
            assertPlaceMarking(i_23_t1_tb_12_t3, 0, 0, 0, 1, 1, 0);
            assertTimeMarking(i_23_t1_tb_12_t3, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_tb_12_t3), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_tb_12_t3));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.makeTimeStep(i_23_t1_tb_12_t3));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_tb_2));
            
            const NetState i_23_t1_tb_12_t3_tb = rule.fireTransition(tb, i_23_t1_tb_12_t3);
            assertPlaceMarking(i_23_t1_tb_12_t3_tb, 0, 0, 0, 1, 1, 1);
            assertTimeMarking(i_23_t1_tb_12_t3_tb, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_tb_12_t3_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_tb_12_t3_tb));
            ASSERT_EQ(i_23_t1_tb_12_t3_tb, rule.makeTimeStep(i_23_t1_tb_12_t3_tb));
            
            // Fire external transition ta:
            const NetState i_23_t1_ta = rule.fireTransition(ta, i_23_t1);
            assertPlaceMarking(i_23_t1_ta, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(i_23_t1_ta, DT, 0, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta));
            
            const NetState i_23_t1_ta_tb = rule.fireTransition(tb, i_23_t1_ta);
            assertPlaceMarking(i_23_t1_ta_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_tb, DT, 0, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_tb));
            ASSERT_EQ(i_23_t1_ta_tb, rule.fireTransition(ta, i_23_t1_tb));
            
            const NetState i_23_t1_ta_tb_1 = rule.makeTimeStep(i_23_t1_ta_tb);
            assertPlaceMarking(i_23_t1_ta_tb_1, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_tb_1, DT, 1, 1, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_tb_1), false, false, true, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_tb_1));
            ASSERT_EQ(i_23_t1_ta_tb_1, rule.fireTransition(ta, i_23_t1_tb_1));
            
            const NetState i_23_t1_ta_tb_2 = rule.makeTimeStep(i_23_t1_ta_tb_1);
            assertPlaceMarking(i_23_t1_ta_tb_2, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_tb_2, DT, 2, 2, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_tb_2), false, false, true, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_tb_2));
            ASSERT_EQ(i_23_t1_ta_tb_2, rule.fireTransition(ta, i_23_t1_tb_2));
            
            const NetState i_23_t1_ta_tb_12_t3 = rule.fireTransition(t3, i_23_t1_ta_tb_1);
            assertPlaceMarking(i_23_t1_ta_tb_12_t3, 0, 0, 0, 1, 0, 0);
            assertTimeMarking(i_23_t1_ta_tb_12_t3, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_tb_12_t3), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_tb_12_t3));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.makeTimeStep(i_23_t1_ta_tb_12_t3));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(ta, i_23_t1_tb_12_t3));
            
            const NetState i_23_t1_ta_tb_12_t3_tb = rule.fireTransition(tb, i_23_t1_ta_tb_12_t3);
            assertPlaceMarking(i_23_t1_ta_tb_12_t3_tb, 0, 0, 0, 1, 0, 1);
            assertTimeMarking(i_23_t1_ta_tb_12_t3_tb, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_tb_12_t3_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_tb_12_t3_tb));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3_tb, rule.makeTimeStep(i_23_t1_ta_tb_12_t3_tb));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3_tb, rule.fireTransition(ta, i_23_t1_tb_12_t3_tb));

            // Here be the deadlocks and the potential deadlocks
            // The following states are reached by passing time in state i_23_t1:
            const NetState i_23_t1_1 = rule.makeTimeStep(i_23_t1);
            assertPlaceMarking(i_23_t1_1, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(i_23_t1_1, DT, 1, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_1), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_1));
            
            const NetState i_23_t1_2 = rule.makeTimeStep(i_23_t1_1);
            assertPlaceMarking(i_23_t1_2, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(i_23_t1_2, DT, 2, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_2), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_2));
            
            const NetState i_23_t1_3 = rule.makeTimeStep(i_23_t1_2);
            assertPlaceMarking(i_23_t1_3, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(i_23_t1_3, DT, 3, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_3), false, true, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_3));
            
            const NetState i_23_t1_4 = rule.makeTimeStep(i_23_t1_3);
            assertPlaceMarking(i_23_t1_4, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(i_23_t1_4, DT, 4, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_4), false, true, false, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_4));
            
            // The following states are reached by passing time in state i_23_t1_ta:
            const NetState i_23_t1_ta_1 = rule.makeTimeStep(i_23_t1_ta);
            assertPlaceMarking(i_23_t1_ta_1, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(i_23_t1_ta_1, DT, 1, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_1), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_1));
            ASSERT_EQ(i_23_t1_ta_1, rule.fireTransition(ta, i_23_t1_1));
            
            const NetState i_23_t1_ta_2 = rule.makeTimeStep(i_23_t1_ta_1);
            assertPlaceMarking(i_23_t1_ta_2, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(i_23_t1_ta_2, DT, 2, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_2), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_2));
            ASSERT_EQ(i_23_t1_ta_2, rule.fireTransition(ta, i_23_t1_2));
            
            const NetState i_23_t1_ta_3 = rule.makeTimeStep(i_23_t1_ta_2);
            assertPlaceMarking(i_23_t1_ta_3, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(i_23_t1_ta_3, DT, 3, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_3), false, true, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_3));
            ASSERT_EQ(i_23_t1_ta_3, rule.fireTransition(ta, i_23_t1_3));
            
            const NetState i_23_t1_ta_4 = rule.makeTimeStep(i_23_t1_ta_3);
            assertPlaceMarking(i_23_t1_ta_4, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(i_23_t1_ta_4, DT, 4, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_4), false, true, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_4));
            ASSERT_EQ(i_23_t1_ta_4, rule.fireTransition(ta, i_23_t1_4));
            
            // The following states are reachable by firing tb from i_23_t1_1 to i_23_t1_4
            const NetState i_23_t1_1_tb = rule.fireTransition(tb, i_23_t1_1);
            assertPlaceMarking(i_23_t1_1_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_1_tb, DT, 1, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_1_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_1_tb));

            const NetState i_23_t1_2_tb = rule.fireTransition(tb, i_23_t1_2);
            assertPlaceMarking(i_23_t1_2_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_2_tb, DT, 2, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_2_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_2_tb));
            
            const NetState i_23_t1_3_tb = rule.fireTransition(tb, i_23_t1_3);
            assertPlaceMarking(i_23_t1_3_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_3_tb, DT, 3, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_3_tb), false, true, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_3_tb));
            
            const NetState i_23_t1_4_tb = rule.fireTransition(tb, i_23_t1_4);
            assertPlaceMarking(i_23_t1_4_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_4_tb, DT, 4, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_4_tb), false, true, false, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_4_tb));
            
            // The following states are reachable by firing tb at i_23_t1_ta_1 ... i_23_t1_ta_4 or by firing ta at i_23_t1_1_tb ... i_23_t1_4_tb
            const NetState i_23_t1_ta_1_tb = rule.fireTransition(tb, i_23_t1_ta_1);
            assertPlaceMarking(i_23_t1_ta_1_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_1_tb, DT, 1, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_1_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_1_tb));
            ASSERT_EQ(i_23_t1_ta_1_tb, rule.fireTransition(ta, i_23_t1_1_tb));

            const NetState i_23_t1_ta_2_tb = rule.fireTransition(tb, i_23_t1_ta_2);
            assertPlaceMarking(i_23_t1_ta_2_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_2_tb, DT, 2, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_2_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_2_tb));
            ASSERT_EQ(i_23_t1_ta_2_tb, rule.fireTransition(ta, i_23_t1_2_tb));
            
            const NetState i_23_t1_ta_3_tb = rule.fireTransition(tb, i_23_t1_ta_3);
            assertPlaceMarking(i_23_t1_ta_3_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_3_tb, DT, 3, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_3_tb), false, true, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_3_tb));
            ASSERT_EQ(i_23_t1_ta_3_tb, rule.fireTransition(ta, i_23_t1_3_tb));
            
            const NetState i_23_t1_ta_4_tb = rule.fireTransition(tb, i_23_t1_ta_4);
            assertPlaceMarking(i_23_t1_ta_4_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_4_tb, DT, 4, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_4_tb), false, true, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_4_tb));
            ASSERT_EQ(i_23_t1_ta_4_tb, rule.fireTransition(ta, i_23_t1_4_tb));
            
            // The following states are reachable by passing time at i_23_t1_1_tb ... i_23_t1_3_tb
            const NetState i_23_t1_1_tb_1 = rule.makeTimeStep(i_23_t1_1_tb);
            assertPlaceMarking(i_23_t1_1_tb_1, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_1_tb_1, DT, 2, 1, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_1_tb_1), false, false, true, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_1_tb_1));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_1_tb_1));

            const NetState i_23_t1_2_tb_1 = rule.makeTimeStep(i_23_t1_2_tb);
            assertPlaceMarking(i_23_t1_2_tb_1, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_2_tb_1, DT, 3, 1, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_2_tb_1), false, true, true, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_2_tb_1));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_2_tb_1));
            
            const NetState i_23_t1_3_tb_1 = rule.makeTimeStep(i_23_t1_3_tb);
            assertPlaceMarking(i_23_t1_3_tb_1, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_3_tb_1, DT, 4, 1, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_3_tb_1), false, true, true, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_3_tb_1));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_3_tb_1));
            
            // The following states are reachable by passing time at i_23_t1_ta_1_tb ... i_23_t1_ta_3_tb
            const NetState i_23_t1_ta_1_tb_1 = rule.makeTimeStep(i_23_t1_ta_1_tb);
            assertPlaceMarking(i_23_t1_ta_1_tb_1, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_1_tb_1, DT, 2, 1, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_1_tb_1), false, false, true, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_1_tb_1));
            ASSERT_EQ(i_23_t1_ta_1_tb_1, rule.fireTransition(ta, i_23_t1_1_tb_1));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(t3, i_23_t1_ta_1_tb_1));

            const NetState i_23_t1_ta_2_tb_1 = rule.makeTimeStep(i_23_t1_ta_2_tb);
            assertPlaceMarking(i_23_t1_ta_2_tb_1, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_2_tb_1, DT, 3, 1, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_2_tb_1), false, true, true, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_ta_2_tb_1));
            ASSERT_EQ(i_23_t1_ta_2_tb_1, rule.fireTransition(ta, i_23_t1_2_tb_1));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(t3, i_23_t1_ta_2_tb_1));
            
            const NetState i_23_t1_ta_3_tb_1 = rule.makeTimeStep(i_23_t1_ta_3_tb);
            assertPlaceMarking(i_23_t1_ta_3_tb_1, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_3_tb_1, DT, 4, 1, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_3_tb_1), false, true, true, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_3_tb_1));
            ASSERT_EQ(i_23_t1_ta_3_tb_1, rule.fireTransition(ta, i_23_t1_3_tb_1));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(t3, i_23_t1_ta_3_tb_1));
            
            // The following states are reachable by passing time at i_23_t1_1_tb_1 and i_23_t1_2_tb_1
            const NetState i_23_t1_1_tb_2 = rule.makeTimeStep(i_23_t1_1_tb_1);
            assertPlaceMarking(i_23_t1_1_tb_2, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_1_tb_2, DT, 3, 2, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_1_tb_2), false, true, true, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_1_tb_2));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_1_tb_2));

            const NetState i_23_t1_2_tb_2 = rule.makeTimeStep(i_23_t1_2_tb_1);
            assertPlaceMarking(i_23_t1_2_tb_2, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(i_23_t1_2_tb_2, DT, 4, 2, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_2_tb_2), false, true, true, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_2_tb_2));
            ASSERT_EQ(i_23_t1_tb_12_t3, rule.fireTransition(t3, i_23_t1_2_tb_2));
            
            // The following states are reachable by passing time at i_23_t1_ta_1_tb_1 and i_23_t1_ta_2_tb_1
            const NetState i_23_t1_ta_1_tb_2 = rule.makeTimeStep(i_23_t1_ta_1_tb_1);
            assertPlaceMarking(i_23_t1_ta_1_tb_2, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_1_tb_2, DT, 3, 2, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_1_tb_2), false, true, true, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_1_tb_2));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(t3, i_23_t1_ta_1_tb_2));

            const NetState i_23_t1_ta_2_tb_2 = rule.makeTimeStep(i_23_t1_ta_2_tb_1);
            assertPlaceMarking(i_23_t1_ta_2_tb_2, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(i_23_t1_ta_2_tb_2, DT, 4, 2, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_ta_2_tb_2), false, true, true, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(i_23_t1_ta_2_tb_2));
            ASSERT_EQ(i_23_t1_ta_tb_12_t3, rule.fireTransition(t3, i_23_t1_ta_2_tb_2));
            
            // The following states are the deadlock states, which are reachable from many of the previously created states.
            // This state is reachable by firing t2 at states i_23_t1_3 or i_23_t1_4
            const NetState i_23_t1_34_t2 = rule.fireTransition(t2, i_23_t1_3); // Ca
            assertPlaceMarking(i_23_t1_34_t2, 0, 0, 1, 0, 1, 0);
            assertTimeMarking(i_23_t1_34_t2, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_34_t2), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_34_t2));
            ASSERT_EQ(i_23_t1_34_t2, rule.makeTimeStep(i_23_t1_34_t2));
            ASSERT_EQ(i_23_t1_34_t2, rule.fireTransition(t2, i_23_t1_4));
            
            // This state is reachable by firing tb at i_23_t1_34_t2
            const NetState i_23_t1_34_t2_tb = rule.fireTransition(tb, i_23_t1_34_t2); // Cab
            assertPlaceMarking(i_23_t1_34_t2_tb, 0, 0, 1, 0, 1, 1);
            assertTimeMarking(i_23_t1_34_t2_tb, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_34_t2_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_34_t2_tb));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.makeTimeStep(i_23_t1_34_t2_tb));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_3_tb));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_4_tb));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_1_tb_2));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_2_tb_1));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_2_tb_2));
            ASSERT_EQ(i_23_t1_34_t2_tb, rule.fireTransition(t2, i_23_t1_3_tb_1));
            
            // This state is reachable by firing ta at i_23_t1_34_t2
            const NetState i_23_t1_34_t2_ta = rule.fireTransition(ta, i_23_t1_34_t2); // C
            assertPlaceMarking(i_23_t1_34_t2_ta, 0, 0, 1, 0, 0, 0);
            assertTimeMarking(i_23_t1_34_t2_ta, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_34_t2_ta), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_34_t2_ta));
            ASSERT_EQ(i_23_t1_34_t2_ta, rule.makeTimeStep(i_23_t1_34_t2_ta));
            ASSERT_EQ(i_23_t1_34_t2_ta, rule.fireTransition(t2, i_23_t1_ta_4));
            ASSERT_EQ(i_23_t1_34_t2_ta, rule.fireTransition(ta, i_23_t1_34_t2));

            // This state is reachable by firing ta at i_23_t1_34_t2_tb or by firing ta at i_23_t1_34_t2_tb
            const NetState i_23_t1_34_t2_ta_tb = rule.fireTransition(tb, i_23_t1_34_t2_ta); // Cb
            assertPlaceMarking(i_23_t1_34_t2_ta_tb, 0, 0, 1, 0, 0, 1);
            assertTimeMarking(i_23_t1_34_t2_ta_tb, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(i_23_t1_34_t2_ta_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(i_23_t1_34_t2_ta_tb));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.makeTimeStep(i_23_t1_34_t2_ta_tb));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(ta, i_23_t1_34_t2_tb));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_1_tb_2));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_2_tb_1));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_2_tb_2));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_3_tb));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_3_tb_1));
            ASSERT_EQ(i_23_t1_34_t2_ta_tb, rule.fireTransition(t2, i_23_t1_ta_4_tb));
        }
    }
}
