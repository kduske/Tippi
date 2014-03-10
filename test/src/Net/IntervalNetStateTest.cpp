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

#include "StringUtils.h"
#include "Net/IntervalNet.h"
#include "Net/IntervalNetParser.h"
#include "Net/IntervalNetState.h"
#include "Net/Marking.h"

namespace Tippi {
    namespace Interval {
        class IntervalNetStateTest : public ::testing::Test {
        protected:
            const Net* m_net;
        public:
            IntervalNetStateTest() :
            m_net(NULL) {
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
        
            ~IntervalNetStateTest() {
                delete m_net;
                m_net = NULL;
            }
        };
        
        TEST_F(IntervalNetStateTest, createInitialState) {
            const NetState initialState = NetState::createInitialState(*m_net);
            ASSERT_EQ(1u, initialState.getPlaceMarking(m_net->findPlace("A")));
            ASSERT_EQ(0u, initialState.getPlaceMarking(m_net->findPlace("B")));
            ASSERT_EQ(0u, initialState.getPlaceMarking(m_net->findPlace("C")));
            ASSERT_EQ(0u, initialState.getPlaceMarking(m_net->findPlace("D")));
            ASSERT_EQ(0u, initialState.getPlaceMarking(m_net->findPlace("a")));
            ASSERT_EQ(0u, initialState.getPlaceMarking(m_net->findPlace("b")));

            ASSERT_EQ(0u, initialState.getTimeMarking(m_net->findTransition("t1")));
            ASSERT_EQ(NetState::DisabledTransition, initialState.getTimeMarking(m_net->findTransition("t2")));
            ASSERT_EQ(NetState::DisabledTransition, initialState.getTimeMarking(m_net->findTransition("t3")));
        }

        TEST_F(IntervalNetStateTest, isPlaceEnabled) {
            const NetState initialState = NetState::createInitialState(*m_net);
            ASSERT_TRUE(initialState.isPlaceEnabled(m_net->findTransition("t1")));
            ASSERT_FALSE(initialState.isPlaceEnabled(m_net->findTransition("t2")));
            ASSERT_FALSE(initialState.isPlaceEnabled(m_net->findTransition("t3")));
        }
        
        TEST_F(IntervalNetStateTest, isTimeEnabled) {
            NetState state = NetState::createInitialState(*m_net);
            ASSERT_FALSE(state.isTimeEnabled(m_net->findTransition("t1")));
            state.makeTimeStep(2, m_net->findTransition("t1"));
            ASSERT_TRUE(state.isTimeEnabled(m_net->findTransition("t1")));
        }

        TEST_F(IntervalNetStateTest, canMakeTimeStep) {
            const NetState initialState = NetState::createInitialState(*m_net);
            ASSERT_TRUE(initialState.canMakeTimeStep(1, m_net->findTransition("t1")));
            ASSERT_TRUE(initialState.canMakeTimeStep(2, m_net->findTransition("t1")));
            ASSERT_TRUE(initialState.canMakeTimeStep(3, m_net->findTransition("t1")));
            ASSERT_FALSE(initialState.canMakeTimeStep(4, m_net->findTransition("t1")));
        }

        TEST_F(IntervalNetStateTest, makeTimeStep) {
            NetState state = NetState::createInitialState(*m_net);
            state.makeTimeStep(2, m_net->findTransition("t1"));
            
            ASSERT_EQ(1u, state.getPlaceMarking(m_net->findPlace("A")));
            ASSERT_EQ(0u, state.getPlaceMarking(m_net->findPlace("B")));
            ASSERT_EQ(0u, state.getPlaceMarking(m_net->findPlace("C")));
            ASSERT_EQ(0u, state.getPlaceMarking(m_net->findPlace("D")));
            ASSERT_EQ(0u, state.getPlaceMarking(m_net->findPlace("a")));
            ASSERT_EQ(0u, state.getPlaceMarking(m_net->findPlace("b")));
            
            ASSERT_EQ(2u, state.getTimeMarking(m_net->findTransition("t1")));
            ASSERT_EQ(NetState::DisabledTransition, state.getTimeMarking(m_net->findTransition("t2")));
            ASSERT_EQ(NetState::DisabledTransition, state.getTimeMarking(m_net->findTransition("t3")));
        }
    }
}
