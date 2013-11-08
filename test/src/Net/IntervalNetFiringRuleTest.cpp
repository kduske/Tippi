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
                                 "INPUT b;\n"
                                 "OUTPUT a;\n"
                                 "MARKING A:1;\n"
                                 "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE B:1,a:1;\n"
                                 "TRANSITION t2 TIME 3,4; CONSUME B:1; PRODUCE C:1;\n"
                                 "TRANSITION t3 TIME 1,2; CONSUME B:1,b:1; PRODUCE D:1;\n"
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
            
            void assertTimeMarking(const NetState& state, const size_t t1, const size_t t2, const size_t t3) {
                ASSERT_EQ(t1, state.getTimeMarking(m_net->findTransition("t1")));
                ASSERT_EQ(t2, state.getTimeMarking(m_net->findTransition("t2")));
                ASSERT_EQ(t3, state.getTimeMarking(m_net->findTransition("t3")));
            }
            
            void assertFireableTransitions(const Transition::List& transitions, const bool t1, const bool t2, const bool t3) {
                size_t count = 0;
                if (t1)
                    ++count;
                if (t2)
                    ++count;
                if (t3)
                    ++count;
                
                ASSERT_EQ(count, transitions.size());
                if (t1)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t1")));
                if (t2)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t2")));
                if (t3)
                    ASSERT_TRUE(VectorUtils::contains(transitions, m_net->findTransition("t3")));
            }
        };
        
        TEST_F(IntervalNetFiringRuleTest, testFullNetExecution) {
            const Transition* t1 = m_net->findTransition("t1");
            const Transition* t2 = m_net->findTransition("t2");
            const Transition* t3 = m_net->findTransition("t3");
            
            const FiringRule rule(*m_net);
            
            const NetState init = NetState::createInitialState(*m_net);
            assertPlaceMarking(init, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init, 0, DT, DT);
            assertFireableTransitions(rule.getFireableTransitions(init), false, false, false);
            ASSERT_TRUE(rule.canMakeTimeStep(init));

            const NetState init_1 = rule.makeTimeStep(init);
            assertPlaceMarking(init_1, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_1, 1, DT, DT);
            assertFireableTransitions(rule.getFireableTransitions(init_1), false, false, false);
            ASSERT_TRUE(rule.canMakeTimeStep(init_1));

            const NetState init_2 = rule.makeTimeStep(init_1);
            assertPlaceMarking(init_2, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_2, 2, DT, DT);
            assertFireableTransitions(rule.getFireableTransitions(init_2), true, false, false);
            ASSERT_TRUE(rule.canMakeTimeStep(init_2));
            
                const NetState init_2_t1 = rule.fireTransition(t1, init_2);
                assertPlaceMarking(init_2_t1, 0, 1, 0, 0, 1, 0);
                assertTimeMarking(init_2_t1, DT, 0, DT);
                assertFireableTransitions(rule.getFireableTransitions(init_2_t1), false, false, false);
                ASSERT_TRUE(rule.canMakeTimeStep(init_2_t1));
            
                const NetState init_2_t1_1 = rule.makeTimeStep(init_2_t1);
                assertPlaceMarking(init_2_t1_1, 0, 1, 0, 0, 1, 0);
                assertTimeMarking(init_2_t1_1, DT, 1, DT);
                assertFireableTransitions(rule.getFireableTransitions(init_2_t1_1), false, false, false);
                ASSERT_TRUE(rule.canMakeTimeStep(init_2_t1_1));

                const NetState init_2_t1_2 = rule.makeTimeStep(init_2_t1_1);
                assertPlaceMarking(init_2_t1_2, 0, 1, 0, 0, 1, 0);
                assertTimeMarking(init_2_t1_2, DT, 2, DT);
                assertFireableTransitions(rule.getFireableTransitions(init_2_t1_2), false, false, false);
                ASSERT_TRUE(rule.canMakeTimeStep(init_2_t1_2));

                const NetState init_2_t1_3 = rule.makeTimeStep(init_2_t1_2);
                assertPlaceMarking(init_2_t1_3, 0, 1, 0, 0, 1, 0);
                assertTimeMarking(init_2_t1_3, DT, 3, DT);
                assertFireableTransitions(rule.getFireableTransitions(init_2_t1_3), false, true, false);
                ASSERT_TRUE(rule.canMakeTimeStep(init_2_t1_3));
            
                const NetState init_2_t1_4 = rule.makeTimeStep(init_2_t1_3);
                assertPlaceMarking(init_2_t1_4, 0, 1, 0, 0, 1, 0);
                assertTimeMarking(init_2_t1_4, DT, 4, DT);
                assertFireableTransitions(rule.getFireableTransitions(init_2_t1_4), false, true, false);
                ASSERT_FALSE(rule.canMakeTimeStep(init_2_t1_4));
            
            const NetState init_3 = rule.makeTimeStep(init_2);
            assertPlaceMarking(init_3, 1, 0, 0, 0, 0, 0);
            assertTimeMarking(init_3, 3, DT, DT);
            assertFireableTransitions(rule.getFireableTransitions(init_3), true, false, false);
            ASSERT_FALSE(rule.canMakeTimeStep(init_3));
         
        }
    }
}
