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

#include "Net/IntervalNetParser.h"
#include "Net/IntervalNet.h"
#include "StringUtils.h"

namespace Tippi {
    namespace Interval {
        
        TEST(IntervalNetParserTest, testEmptyNet) {
            const Net* net = NULL;
            
            net = NetParser("TIMENET").parse();
            ASSERT_TRUE(net != NULL);
            ASSERT_TRUE(net->getPlaces().empty());
            ASSERT_TRUE(net->getTransitions().empty());
            ASSERT_TRUE(net->getFinalMarkings().empty());
            
            net = NetParser("TIMENET\n").parse();
            ASSERT_TRUE(net != NULL);
            ASSERT_TRUE(net->getPlaces().empty());
            ASSERT_TRUE(net->getTransitions().empty());
            ASSERT_TRUE(net->getFinalMarkings().empty());
        }
        
        TEST(IntervalNetParserTest, testSafeNetWithImplicitBound) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n");
            
            NetParser parser(str);
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);
            
            ASSERT_EQ(1u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            ASSERT_EQ(1u, A->getBound());
            delete net;
        }
        
        TEST(IntervalNetParserTest, testSafeNetWithExplicitBounds) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE 3 : A,B; SAFE 2 : C;\n");
            
            NetParser parser(str);
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);
            
            ASSERT_EQ(3u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            const Place* B = net->findPlace("B");
            const Place* C = net->findPlace("C");
            
            ASSERT_EQ(3u, A->getBound());
            ASSERT_EQ(3u, B->getBound());
            ASSERT_EQ(2u, C->getBound());
            delete net;
        }
        
        TEST(IntervalNetParserTest, testUnsafeNet) {
            const String str("TIMENET\n"
                             "PLACE A;\n");
            
            NetParser parser(str);
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);
            
            ASSERT_EQ(1u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            ASSERT_EQ(0u, A->getBound());

            delete net;
        }

        
        TEST(IntervalNetParserTest, testMixedNet) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE 3 : A,B; SAFE 2 : C; D;\n");
            
            NetParser parser(str);
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);
            
            ASSERT_EQ(4u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            const Place* B = net->findPlace("B");
            const Place* C = net->findPlace("C");
            const Place* D = net->findPlace("D");
            
            ASSERT_EQ(3u, A->getBound());
            ASSERT_EQ(3u, B->getBound());
            ASSERT_EQ(2u, C->getBound());
            ASSERT_EQ(0u, D->getBound());
            delete net;
        }

        TEST(IntervalNetParserTest, testSmallExample) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n"
                             "MARKING A:1;\n"
                             "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE A:1;\n"
                             "FINALMARKING A:0;");
            
            NetParser parser(str);
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);
            
            ASSERT_EQ(1u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            
            ASSERT_TRUE(A != NULL);
            ASSERT_EQ(1u, A->getBound());
            ASSERT_FALSE(A->isInputPlace());
            ASSERT_FALSE(A->isOutputPlace());
            
            const Marking& initialMarking = net->getInitialMarking();
            ASSERT_EQ(1u, initialMarking[A]);
            
            ASSERT_EQ(1u, net->getTransitions().size());
            const Transition* t1 = net->findTransition("t1");
            
            ASSERT_TRUE(t1 != NULL);
            
            ASSERT_EQ(TimeInterval(2,3), t1->getInterval());
            
            ASSERT_EQ(1u, t1->getIncoming().size());
            ASSERT_TRUE(t1->isInPreset(A));
            ASSERT_EQ(1u, t1->getOutgoing().size());
            ASSERT_TRUE(t1->isInPostset(A));
            
            const Marking::List& finalMarkings = net->getFinalMarkings();
            ASSERT_EQ(1u, finalMarkings.size());
            
            const Marking& finalMarking = finalMarkings.front();
            ASSERT_EQ(0u, finalMarking[A]);
            
            delete net;
        }
        
        
        TEST(IntervalNetParserTest, testMissingPlaceInTransitionPreset) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n"
                             "MARKING A:1;\n"
                             "TRANSITION t1 TIME 2,3; CONSUME A:1,B:1; PRODUCE A:1;\n"
                             "FINALMARKING A:0;");
            
            NetParser parser(str);
            ASSERT_THROW(parser.parse(), ParserException);
        }
        
        TEST(IntervalNetParserTest, testMissingPlaceInTransitionPostset) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n"
                             "MARKING A:1;\n"
                             "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE A:1,B:1;\n"
                             "FINALMARKING A:0;");
            
            NetParser parser(str);
            ASSERT_THROW(parser.parse(), ParserException);
        }
        
        TEST(IntervalNetParserTest, testMissingPlaceInInitialMarking) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n"
                             "MARKING B:1;\n"
                             "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE A:1;\n"
                             "FINALMARKING A:0;");
            
            NetParser parser(str);
            ASSERT_THROW(parser.parse(), ParserException);
        }
        
        TEST(IntervalNetParserTest, testMissingPlaceInFinalMarking) {
            const String str("TIMENET\n"
                             "PLACE\n"
                             "SAFE A;\n"
                             "MARKING A:1;\n"
                             "TRANSITION t1 TIME 2,3; CONSUME A:1; PRODUCE A:1;\n"
                             "FINALMARKING A:0,B:0;");
            
            NetParser parser(str);
            ASSERT_THROW(parser.parse(), ParserException);
        }
        
        TEST(IntervalNetParserTest, testFullExample) {
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
            const Net* net = parser.parse();
            ASSERT_TRUE(net != NULL);

            ASSERT_EQ(6u, net->getPlaces().size());
            const Place* A = net->findPlace("A");
            const Place* B = net->findPlace("B");
            const Place* C = net->findPlace("C");
            const Place* D = net->findPlace("D");
            const Place* a = net->findPlace("a");
            const Place* b = net->findPlace("b");
            
            ASSERT_TRUE(A != NULL);
            ASSERT_EQ(1u, A->getBound());
            ASSERT_FALSE(A->isInputPlace());
            ASSERT_FALSE(A->isOutputPlace());
            
            ASSERT_TRUE(B != NULL);
            ASSERT_EQ(1u, B->getBound());
            ASSERT_FALSE(B->isInputPlace());
            ASSERT_FALSE(B->isOutputPlace());

            ASSERT_TRUE(C != NULL);
            ASSERT_EQ(1u, C->getBound());
            ASSERT_FALSE(C->isInputPlace());
            ASSERT_FALSE(C->isOutputPlace());

            ASSERT_TRUE(D != NULL);
            ASSERT_EQ(1u, D->getBound());
            ASSERT_FALSE(D->isInputPlace());
            ASSERT_FALSE(D->isOutputPlace());

            ASSERT_TRUE(a != NULL);
            ASSERT_EQ(1u, a->getBound());
            ASSERT_FALSE(a->isInputPlace());
            ASSERT_TRUE(a->isOutputPlace());

            ASSERT_TRUE(b != NULL);
            ASSERT_EQ(1u, b->getBound());
            ASSERT_TRUE(b->isInputPlace());
            ASSERT_FALSE(b->isOutputPlace());

            const Marking& initialMarking = net->getInitialMarking();
            ASSERT_EQ(1u, initialMarking[A]);
            ASSERT_EQ(0u, initialMarking[B]);
            ASSERT_EQ(0u, initialMarking[C]);
            ASSERT_EQ(0u, initialMarking[D]);
            ASSERT_EQ(0u, initialMarking[a]);
            ASSERT_EQ(0u, initialMarking[b]);
            
            ASSERT_EQ(3u, net->getTransitions().size());
            const Transition* t1 = net->findTransition("t1");
            const Transition* t2 = net->findTransition("t2");
            const Transition* t3 = net->findTransition("t3");
            
            ASSERT_TRUE(t1 != NULL);
            ASSERT_TRUE(t2 != NULL);
            ASSERT_TRUE(t3 != NULL);
            
            ASSERT_EQ(TimeInterval(2,3), t1->getInterval());
            ASSERT_EQ(TimeInterval(3,4), t2->getInterval());
            ASSERT_EQ(TimeInterval(1,2), t3->getInterval());
            
            ASSERT_EQ(1u, t1->getIncoming().size());
            ASSERT_TRUE(t1->isInPreset(A));
            ASSERT_EQ(2u, t1->getOutgoing().size());
            ASSERT_TRUE(t1->isInPostset(B));
            ASSERT_TRUE(t1->isInPostset(a));
            
            ASSERT_EQ(1u, t2->getIncoming().size());
            ASSERT_TRUE(t2->isInPreset(B));
            ASSERT_EQ(1u, t2->getOutgoing().size());
            ASSERT_TRUE(t2->isInPostset(C));
            
            ASSERT_EQ(2u, t3->getIncoming().size());
            ASSERT_TRUE(t3->isInPreset(B));
            ASSERT_TRUE(t3->isInPreset(b));
            ASSERT_EQ(1u, t3->getOutgoing().size());
            ASSERT_TRUE(t3->isInPostset(D));
            
            const Marking::List& finalMarkings = net->getFinalMarkings();
            ASSERT_EQ(1u, finalMarkings.size());
            
            const Marking& finalMarking = finalMarkings.front();
            ASSERT_EQ(0u, finalMarking[A]);
            ASSERT_EQ(0u, finalMarking[B]);
            ASSERT_EQ(0u, finalMarking[C]);
            ASSERT_EQ(1u, finalMarking[D]);
            ASSERT_EQ(0u, finalMarking[a]);
            ASSERT_EQ(0u, finalMarking[b]);
            
            delete net;
        }
    }
}
