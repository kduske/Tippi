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

#include "CollectionUtils.h"
#include "Net/IntervalNet.h"
#include "Net/IntervalNetFiringRule.h"
#include "Net/IntervalNetParser.h"
#include "Net/IntervalNetState.h"

namespace Tippi {
    namespace Interval {
        class IntervalNetFiringRuleTest : public ::testing::Test {
        protected:
            Net* m_net;
            const size_t DT;
        public:
            IntervalNetFiringRuleTest() :
            m_net(NULL),
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
                m_net = parser.parse();
            }
            
            ~IntervalNetFiringRuleTest() {
                delete m_net;
                m_net = NULL;
            }

            
            void assertPlaceMarking(const NetState& state, const size_t A, const size_t B, const size_t C, const size_t D, const size_t a, const size_t b) {
                ASSERT_EQ(A, state.getPlaceMarking(m_net->findPlace("A")));
                ASSERT_EQ(B, state.getPlaceMarking(m_net->findPlace("B")));
                ASSERT_EQ(C, state.getPlaceMarking(m_net->findPlace("C")));
                ASSERT_EQ(D, state.getPlaceMarking(m_net->findPlace("D")));
                ASSERT_EQ(a, state.getPlaceMarking(m_net->findPlace("a")));
                ASSERT_EQ(b, state.getPlaceMarking(m_net->findPlace("b")));
            }
            
            void assertTimeMarking(const NetState& state, const size_t t1, const size_t t2, const size_t t3, const size_t t4, const size_t t5) {
                ASSERT_EQ(t1, state.getTimeMarking(m_net->findTransition("t1")));
                ASSERT_EQ(t2, state.getTimeMarking(m_net->findTransition("t2")));
                ASSERT_EQ(t3, state.getTimeMarking(m_net->findTransition("t3")));
                ASSERT_EQ(t4, state.getTimeMarking(m_net->findTransition("ta")));
                ASSERT_EQ(t5, state.getTimeMarking(m_net->findTransition("tb")));
            }
            
            void assertFireableTransitions(const Transition::List& transitions, const bool t1, const bool t2, const bool t3, const bool t4, const bool t5) {
                size_t count = 0;
                if (t1)
                    ++count;
                if (t2)
                    ++count;
                if (t3)
                    ++count;
                if (t4)
                    ++count;
                if (t5)
                    ++count;
                
                ASSERT_EQ(count, transitions.size());
                if (t1)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t1")));
                if (t2)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t2")));
                if (t3)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t3")));
                if (t4)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("ta")));
                if (t5)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("tb")));
            }
        };
        
        TEST_F(IntervalNetFiringRuleTest, testFullNetExecution) {
            const Transition* t1 = m_net->findTransition("t1");
            const Transition* t2 = m_net->findTransition("t2");
            const Transition* t3 = m_net->findTransition("t3");
            const Transition* ta = m_net->findTransition("ta");
            const Transition* tb = m_net->findTransition("tb");
            
            const FiringRule rule(*m_net);
            
            const NetState init = NetState::createInitialState(*m_net);
            assertPlaceMarking(init, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init, 0, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init));
            
            const NetState init_1 = rule.makeTimeStep(init);
            assertPlaceMarking(init_1, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_1, 1, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_1), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_1));

            const NetState init_2 = rule.makeTimeStep(init_1);
            assertPlaceMarking(init_2, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_2, 2, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_2), true, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_2));

            const NetState init_3 = rule.makeTimeStep(init_2);
            assertPlaceMarking(init_3, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_3, 3, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_3), true, false, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(init_3));
         
            const NetState init_23_t1 = rule.fireTransition(t1, init_2);
            assertPlaceMarking(init_23_t1, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(init_23_t1, DT, 0, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1));

            // Fire external transition tb:
            const NetState init_tb = rule.fireTransition(tb, init);
            assertPlaceMarking(init_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(init_tb, 0, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_tb));
            
            const NetState init_1_tb = rule.fireTransition(tb, init_1);
            assertPlaceMarking(init_1_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(init_1_tb, 1, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_1_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_1_tb));
            ASSERT_EQ(init_1_tb, rule.makeTimeStep(init_tb));
            
            const NetState init_2_tb = rule.fireTransition(tb, init_2);
            assertPlaceMarking(init_2_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(init_2_tb, 2, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_2_tb), true, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_2_tb));
            ASSERT_EQ(init_2_tb, rule.makeTimeStep(init_1_tb));

            const NetState init_3_tb = rule.fireTransition(tb, init_3);
            assertPlaceMarking(init_3_tb, 1, 0, 0, 0, 0, 1);
            assertTimeMarking(init_3_tb, 3, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_3_tb), true, false, false, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(init_3_tb));
            ASSERT_EQ(init_3_tb, rule.makeTimeStep(init_2_tb));

            const NetState init_23_t1_tb = rule.fireTransition(tb, init_23_t1);
            assertPlaceMarking(init_23_t1_tb, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(init_23_t1_tb, DT, 0, 0, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_tb));
            ASSERT_EQ(init_23_t1_tb, rule.fireTransition(t1, init_2_tb));
            ASSERT_EQ(init_23_t1_tb, rule.fireTransition(t1, init_3_tb));
            
            const NetState init_23_t1_tb_1 = rule.makeTimeStep(init_23_t1_tb);
            assertPlaceMarking(init_23_t1_tb_1, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(init_23_t1_tb_1, DT, 1, 1, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_tb_1), false, false, true, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_tb_1));

            const NetState init_23_t1_tb_2 = rule.makeTimeStep(init_23_t1_tb_1);
            assertPlaceMarking(init_23_t1_tb_2, 0, 1, 0, 0, 1, 1);
            assertTimeMarking(init_23_t1_tb_2, DT, 2, 2, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_tb_2), false, false, true, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(init_23_t1_tb_2));

            const NetState init_23_t1_tb_12_t3 = rule.fireTransition(t3, init_23_t1_tb_1);
            assertPlaceMarking(init_23_t1_tb_12_t3, 0, 0, 0, 1, 1, 0);
            assertTimeMarking(init_23_t1_tb_12_t3, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_tb_12_t3), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_tb_12_t3));
            ASSERT_EQ(init_23_t1_tb_12_t3, rule.makeTimeStep(init_23_t1_tb_12_t3));
            ASSERT_EQ(init_23_t1_tb_12_t3, rule.fireTransition(t3, init_23_t1_tb_2));
            
            const NetState init_23_t1_tb_12_t3_tb = rule.fireTransition(tb, init_23_t1_tb_12_t3);
            assertPlaceMarking(init_23_t1_tb_12_t3_tb, 0, 0, 0, 1, 1, 1);
            assertTimeMarking(init_23_t1_tb_12_t3_tb, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_tb_12_t3_tb), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_tb_12_t3_tb));
            ASSERT_EQ(init_23_t1_tb_12_t3_tb, rule.makeTimeStep(init_23_t1_tb_12_t3_tb));
            
            // Fire external transition ta:
            const NetState init_23_t1_ta = rule.fireTransition(ta, init_23_t1);
            assertPlaceMarking(init_23_t1_ta, 0, 1, 0, 0, 0, 0);
            assertTimeMarking(init_23_t1_ta, DT, 0, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_ta));
            
            const NetState init_23_t1_ta_tb = rule.fireTransition(tb, init_23_t1_ta);
            assertPlaceMarking(init_23_t1_ta_tb, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(init_23_t1_ta_tb, DT, 0, 0, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_ta_tb));
            ASSERT_EQ(init_23_t1_ta_tb, rule.fireTransition(ta, init_23_t1_tb));
            
            const NetState init_23_t1_ta_tb_1 = rule.makeTimeStep(init_23_t1_ta_tb);
            assertPlaceMarking(init_23_t1_ta_tb_1, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(init_23_t1_ta_tb_1, DT, 1, 1, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta_tb_1), false, false, true, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_ta_tb_1));
            ASSERT_EQ(init_23_t1_ta_tb_1, rule.fireTransition(ta, init_23_t1_tb_1));
            
            const NetState init_23_t1_ta_tb_2 = rule.makeTimeStep(init_23_t1_ta_tb_1);
            assertPlaceMarking(init_23_t1_ta_tb_2, 0, 1, 0, 0, 0, 1);
            assertTimeMarking(init_23_t1_ta_tb_2, DT, 2, 2, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta_tb_2), false, false, true, false, true);
            ASSERT_FALSE(rule.canMakeTimeStep(init_23_t1_ta_tb_2));
            ASSERT_EQ(init_23_t1_ta_tb_2, rule.fireTransition(ta, init_23_t1_tb_2));
            
            const NetState init_23_t1_ta_tb_12_t3 = rule.fireTransition(t3, init_23_t1_ta_tb_1);
            assertPlaceMarking(init_23_t1_ta_tb_12_t3, 0, 0, 0, 1, 0, 0);
            assertTimeMarking(init_23_t1_ta_tb_12_t3, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta_tb_12_t3), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_ta_tb_12_t3));
            ASSERT_EQ(init_23_t1_ta_tb_12_t3, rule.makeTimeStep(init_23_t1_ta_tb_12_t3));
            ASSERT_EQ(init_23_t1_ta_tb_12_t3, rule.fireTransition(ta, init_23_t1_tb_12_t3));
            
            const NetState init_23_t1_ta_tb_12_t3_tb = rule.fireTransition(tb, init_23_t1_ta_tb_12_t3);
            assertPlaceMarking(init_23_t1_ta_tb_12_t3_tb, 0, 0, 0, 1, 0, 1);
            assertTimeMarking(init_23_t1_ta_tb_12_t3_tb, DT, DT, DT, DT, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_ta_tb_12_t3_tb), false, false, false, false, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_ta_tb_12_t3_tb));
            ASSERT_EQ(init_23_t1_ta_tb_12_t3_tb, rule.makeTimeStep(init_23_t1_ta_tb_12_t3_tb));
            ASSERT_EQ(init_23_t1_ta_tb_12_t3_tb, rule.fireTransition(ta, init_23_t1_tb_12_t3_tb));

            // Here be the deadlocks and the potential deadlocks
            const NetState init_23_t1_1 = rule.makeTimeStep(init_23_t1);
            assertPlaceMarking(init_23_t1_1, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(init_23_t1_1, DT, 1, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_1), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_1));
            
            const NetState init_23_t1_2 = rule.makeTimeStep(init_23_t1_1);
            assertPlaceMarking(init_23_t1_2, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(init_23_t1_2, DT, 2, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_2), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_2));
            
            const NetState init_23_t1_3 = rule.makeTimeStep(init_23_t1_2);
            assertPlaceMarking(init_23_t1_3, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(init_23_t1_3, DT, 3, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_3), false, true, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_3));
            
            const NetState init_23_t1_3_t2 = rule.fireTransition(t2, init_23_t1_3);
            assertPlaceMarking(init_23_t1_3_t2, 0, 0, 1, 0, 1, 0);
            assertTimeMarking(init_23_t1_3_t2, DT, DT, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_3_t2), false, false, false, true, true);
            ASSERT_TRUE(rule.canMakeTimeStep(init_23_t1_3_t2));
            
            const NetState init_23_t1_4 = rule.makeTimeStep(init_23_t1_3);
            assertPlaceMarking(init_23_t1_4, 0, 1, 0, 0, 1, 0);
            assertTimeMarking(init_23_t1_4, DT, 4, DT, 0, 0);
            assertFireableTransitions(rule.getFireableTransitions(init_23_t1_4), false, true, false, true, true);
            ASSERT_FALSE(rule.canMakeTimeStep(init_23_t1_4));
            
            ASSERT_EQ(init_23_t1_3_t2, rule.fireTransition(t2, init_23_t1_4));
        }
    }
}
