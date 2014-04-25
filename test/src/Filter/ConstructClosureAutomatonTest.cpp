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

#include "ConstructClosureAutomaton.h"
#include "IntervalNet.h"
#include "Closure.h"

namespace Tippi {
    static bool hasMarking(const ClosureState* state,
                    const size_t A, const size_t B, const size_t C, const size_t D, const size_t a, const size_t b,
                    const size_t t1, const size_t t2, const size_t t3, const size_t ta, const size_t tb) {
        const Interval::NetState test(Marking::createMarking(A, B, C, D, a, b),
                                      Marking::createMarking(t1, t2, t3, ta, tb));
        return state->getClosure().contains(test);
    }
    
    static bool hasMarkings(const ClosureState* state, const size_t count) {
        const Closure& closure = state->getClosure();
        const Interval::NetState::Set& states = closure.getStates();
        const size_t stateCount = states.size();
        return stateCount == count;
    }
    
    static bool isEmptyState(const ClosureState* state) {
        return state->isEmpty();
    }
    
    static bool hasOutgoingEdges(const ClosureState* state, const bool _1, const bool as, const bool ar, const bool bs, const bool br) {
        size_t count = 0;
        if (_1) ++count;
        if (as) ++count;
        if (ar) ++count;
        if (bs) ++count;
        if (br) ++count;

        if (state->getOutgoing().size() != count)
            return false;
        if (_1 && !state->hasOutgoingEdge("1"))
            return false;
        if (as && !state->hasOutgoingEdge("a!"))
            return false;
        if (ar && !state->hasOutgoingEdge("a?"))
            return false;
        if (bs && !state->hasOutgoingEdge("b!"))
            return false;
        if (br && !state->hasOutgoingEdge("b?"))
            return false;
        return true;
    }
    
    static bool hasEmptySuccessors(const ClosureState* state, const bool _1, const bool as, const bool ar, const bool bs, const bool br) {
        if (_1 && !isEmptyState(state->findDirectSuccessor("1")))
            return false;
        if (as && !isEmptyState(state->findDirectSuccessor("a!")))
            return false;
        if (ar && !isEmptyState(state->findDirectSuccessor("a?")))
            return false;
        if (bs && !isEmptyState(state->findDirectSuccessor("b!")))
            return false;
        if (br && !isEmptyState(state->findDirectSuccessor("b?")))
            return false;
        return true;
    }

    TEST(ConstructClosureAutomatonTest, simpleNet) {
        static const size_t DT = Interval::NetState::DisabledTransition;

        using Interval::Net;
        using Interval::Place;
        using Interval::Transition;
        using Interval::TimeInterval;
        
        ConstructClosureAutomaton::NetPtr net(new Net());
        
        Place* A = net->createPlace("A");
        Place* B = net->createPlace("B");
        Place* C = net->createPlace("C");
        Place* D = net->createPlace("D");
        
        Place* a = net->createPlace("a");
        a->setOutputPlace(true);
        Place* b = net->createPlace("b");
        b->setInputPlace(true);
        
        Transition* t1 = net->createTransition("a!", TimeInterval(2,3));
        Transition* t2 = net->createTransition("", TimeInterval(3,4));
        Transition* t3 = net->createTransition("b?", TimeInterval(1,2));
        Transition* ta = net->createTransition("a?", TimeInterval(0, TimeInterval::Infinity));
        Transition* tb = net->createTransition("b!", TimeInterval(0, TimeInterval::Infinity));
        
        net->connect(A, t1);
        net->connect(t1, a);
        net->connect(t1, B);
        net->connect(B, t2);
        net->connect(B, t3);
        net->connect(t2, C);
        net->connect(b, t3);
        net->connect(t3, D);
        net->connect(a, ta);
        net->connect(tb, b);
        
        /*                                                    A  B  C  D  a  b */
        const Marking initialMarking = Marking::createMarking(1, 0, 0, 0, 0, 0);
        const Marking   finalMarking = Marking::createMarking(0, 0, 0, 1, 0, 0);
        
        net->setInitialMarking(initialMarking);
        net->addFinalMarking(finalMarking);
        
        ClosureAutomaton::Ptr cl = ConstructClosureAutomaton()(net);
        const ClosureState* i = cl->getInitialState();
        ASSERT_TRUE(i != NULL);
        ASSERT_FALSE(i->isFinal());
        ASSERT_TRUE(hasMarkings(i, 1));
        ASSERT_TRUE(hasMarking(i,
                               1,  0,  0,  0, 0, 0,
                               0, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i, true, true, true, true, true));
        ASSERT_TRUE(hasEmptySuccessors(i, false, true, true, false, true));
        
        const ClosureState* i_bs = i->findDirectSuccessor("b!");
        ASSERT_FALSE(i_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs, 1));
        ASSERT_TRUE(hasMarking(i_bs,
                               1,  0,  0,  0, 0, 1,
                               0, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs, false, true, true, false, true));

        const ClosureState* i_bs_1 = i_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_1, 1));
        ASSERT_TRUE(hasMarking(i_bs_1,
                               1,  0,  0,  0, 0, 1,
                               1, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_1, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_1, false, true, true, false, true));
        
        const ClosureState* i_bs_2 = i_bs_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_bs_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2, 1));
        ASSERT_TRUE(hasMarking(i_bs_2,
                               1,  0,  0,  0, 0, 1,
                               2, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2, false, false, true, false, true));
        
        const ClosureState* i_bs_3 = i_bs_2->findDirectSuccessor("1");
        ASSERT_FALSE(i_bs_3->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_3, 1));
        ASSERT_TRUE(hasMarking(i_bs_3,
                               1,  0,  0,  0, 0, 1,
                               3, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_3, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_3, true, false, true, false, true));

        const ClosureState* i_bs_2_as = i_bs_2->findDirectSuccessor("a!");
        ASSERT_FALSE(i_bs_2_as->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as,
                                0,  1,  0,  0, 1, 1,
                               DT,  0,  0,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as, false, true, false, false, true));
        ASSERT_TRUE(i_bs_3->findDirectSuccessor("a!") == i_bs_2_as);
        
        const ClosureState* i_bs_2_as_1 = i_bs_2_as->findDirectSuccessor("1");
        ASSERT_FALSE(i_bs_2_as_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_1, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_1,
                               0,  1,  0,  0, 1, 1,
                               DT,  1,  1,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_1, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_1, false, true, false, false, false));
        
        const ClosureState* i_bs_2_as_2 = i_bs_2_as_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_bs_2_as_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2,
                               0,  1,  0,  0, 1, 1,
                               DT,  2,  2,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2, true, true, false, false, false));
        
        const ClosureState* i_bs_2_as_2_ar = i_bs_2_as_2->findDirectSuccessor("a?");
        ASSERT_FALSE(i_bs_2_as_2_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2_ar, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2_ar,
                               0,  1,  0,  0, 0, 1,
                               DT,  2,  2,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2_ar, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2_ar, true, true, true, false, false));
        
        const ClosureState* i_bs_2_as_2_ar_br = i_bs_2_as_2_ar->findDirectSuccessor("b?");
        ASSERT_TRUE(i_bs_2_as_2_ar_br->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2_ar_br, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2_ar_br,
                               0,  0,  0,  1, 0, 0,
                               DT,  DT,  DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2_ar_br, true, true, true, true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2_ar_br, false, true, true, false, true));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_bs_2_as_2_ar_br->findDirectSuccessor("1"));

        const ClosureState* i_bs_2_as_2_ar_br_bs = i_bs_2_as_2_ar_br->findDirectSuccessor("b!");
        ASSERT_FALSE(i_bs_2_as_2_ar_br_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2_ar_br_bs, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2_ar_br_bs,
                               0,  0,  0,  1, 0, 1,
                               DT,  DT,  DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2_ar_br_bs, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2_ar_br_bs, false, true, true, false, true));
        ASSERT_EQ(i_bs_2_as_2_ar_br_bs, i_bs_2_as_2_ar_br_bs->findDirectSuccessor("1"));
        
        const ClosureState* i_bs_2_as_2_br = i_bs_2_as_2->findDirectSuccessor("b?");
        ASSERT_FALSE(i_bs_2_as_2_br->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2_br, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2_br,
                               0,  0,  0,  1, 1, 0,
                               DT,  DT,  DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2_br, true, true, true, true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2_br, false, true, false, false, true));
        ASSERT_EQ(i_bs_2_as_2_br, i_bs_2_as_1->findDirectSuccessor("b?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_bs_2_as_2_br->findDirectSuccessor("1"));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_bs_2_as_2_br->findDirectSuccessor("a?"));
        
        const ClosureState* i_bs_2_as_2_br_bs = i_bs_2_as_2_br->findDirectSuccessor("b!");
        ASSERT_FALSE(i_bs_2_as_2_br_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_2_br_bs, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_2_br_bs,
                               0,  0,  0,  1, 1, 1,
                               DT,  DT,  DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_2_br_bs, true, true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_2_br_bs, false, true, false, false, true));
        ASSERT_EQ(i_bs_2_as_2_br_bs, i_bs_2_as_2_br_bs->findDirectSuccessor("1"));
        ASSERT_EQ(i_bs_2_as_2_ar_br_bs, i_bs_2_as_2_br_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_bs_2_as_1_ar = i_bs_2_as_1->findDirectSuccessor("a?");
        ASSERT_FALSE(i_bs_2_as_1_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_1_ar, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_1_ar,
                               0,  1,  0,  0, 0, 1,
                               DT,  1,  1,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_1_ar,   true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_1_ar, false, true, true, false, false));
        ASSERT_EQ(i_bs_2_as_2_ar, i_bs_2_as_1_ar->findDirectSuccessor("1"));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_bs_2_as_1_ar->findDirectSuccessor("b?"));

        const ClosureState* i_bs_2_as_ar = i_bs_2_as->findDirectSuccessor("a?");
        ASSERT_FALSE(i_bs_2_as_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_bs_2_as_ar, 1));
        ASSERT_TRUE(hasMarking(i_bs_2_as_ar,
                               0,  1,  0,  0, 0, 1,
                               DT,  0,  0,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_bs_2_as_ar,   true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_bs_2_as_ar, false, true, true, false, true));
        ASSERT_EQ(i_bs_2_as_1_ar, i_bs_2_as_ar->findDirectSuccessor("1"));
        
        
        const ClosureState* i_1 = i->findDirectSuccessor("1");
        ASSERT_FALSE(i_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_1, 1));
        ASSERT_TRUE(hasMarking(i_1,
                               1,  0,  0,  0, 0, 0,
                               1, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_1, true, true, true, true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_1, false, true, true, false, true));
        ASSERT_EQ(i_bs_1, i_1->findDirectSuccessor("b!"));
        
        const ClosureState* i_2 = i_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2, 1));
        ASSERT_TRUE(hasMarking(i_2,
                               1,  0,  0,  0, 0, 0,
                               2, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2,    true,  true, true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2, false, false, true, false, true));
        ASSERT_EQ(i_bs_2, i_2->findDirectSuccessor("b!"));
        
        const ClosureState* i_3 = i_2->findDirectSuccessor("1");
        ASSERT_FALSE(i_3->isFinal());
        ASSERT_TRUE(hasMarkings(i_3, 1));
        ASSERT_TRUE(hasMarking(i_3,
                               1,  0,  0,  0, 0, 0,
                               3, DT, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_3,    true,  true, true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_3,  true, false, true, false, true));
        ASSERT_EQ(i_bs_3, i_3->findDirectSuccessor("b!"));
        
        const ClosureState* i_2_as = i_2->findDirectSuccessor("a!");
        ASSERT_FALSE(i_2_as->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as, 1));
        ASSERT_TRUE(hasMarking(i_2_as,
                               0,  1,  0,  0, 1, 0,
                               DT, 0, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as, false,  true, false, false, true));
        ASSERT_EQ(i_2_as, i_3->findDirectSuccessor("a!"));
        ASSERT_EQ(i_bs_2_as, i_2_as->findDirectSuccessor("b!"));

        const ClosureState* i_2_as_1 = i_2_as->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1,
                               0,  1,  0,  0, 1, 0,
                               DT, 1, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1, false,  true, false, false, true));

        
        const ClosureState* i_2_as_ar = i_2_as->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_ar, 1));
        ASSERT_TRUE(hasMarking(i_2_as_ar,
                               0,  1,  0,  0, 0, 0,
                               DT, 0, DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_ar,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_ar, false,  true, true, false, true));
        ASSERT_EQ(i_bs_2_as_ar, i_2_as_ar->findDirectSuccessor("b!"));
        
        const ClosureState* i_2_as_2 = i_2_as_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2, 1));
        ASSERT_TRUE(hasMarking(i_2_as_2,
                               0,  1,  0,  0, 1, 0,
                               DT, 2, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2, false,  true, false, false, true));
        
        const ClosureState* i_2_as_3 = i_2_as_2->findDirectSuccessor("1");
        const String test = i_2_as_3->getClosure().asString(";", " - ");
        ASSERT_FALSE(i_2_as_3->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3,
                               0,  0,  1,  0, 1, 0,
                               DT, DT, DT,  0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3,
                               0,  1,  0,  0, 1, 0,
                               DT, 3, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3, false,  true, false, false, true));
        
        const ClosureState* i_2_as_4 = i_2_as_3->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_4->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_4, 2));
        ASSERT_TRUE(hasMarking(i_2_as_4,
                               0,  0,  1,  0, 1, 0,
                               DT, DT, DT,  0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_4,
                               0,  1,  0,  0, 1, 0,
                               DT, 4, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_4,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_4, false,  true, false, false, true));

        const ClosureState* i_2_as_5 = i_2_as_4->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_5->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_5, 1));
        ASSERT_TRUE(hasMarking(i_2_as_5,
                               0,  0,  1,  0, 1, 0,
                               DT, DT, DT,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_5,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_5, false,  true, false, false, true));
        ASSERT_EQ(i_2_as_5, i_2_as_5->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_5_ar = i_2_as_5->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_5_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_5_ar, 1));
        ASSERT_TRUE(hasMarking(i_2_as_5_ar,
                               0,  0,  1,  0, 0, 0,
                               DT, DT, DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_5_ar,    true,  true,  true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_5_ar, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_5_ar, i_2_as_5_ar->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_5_ar_bs = i_2_as_5_ar->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_5_ar_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_5_ar_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_5_ar_bs,
                               0,  0,  1,  0, 0, 1,
                               DT, DT, DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_5_ar_bs,    true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_5_ar_bs, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_5_ar_bs->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_5_bs = i_2_as_5->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_5_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_5_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_5_bs,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_5_bs,    true,  true,  true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_5_bs, false,  true, false, false, true));
        ASSERT_EQ(i_2_as_5_bs, i_2_as_5_bs->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_5_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_2_as_4_ar = i_2_as_4->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_4_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_4_ar, 2));
        ASSERT_TRUE(hasMarking(i_2_as_4_ar,
                               0,  0,  1,  0, 0, 0,
                               DT, DT, DT,  DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_4_ar,
                               0,  1,  0,  0, 0, 0,
                               DT, 4, DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_4_ar,    true,  true, true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_4_ar, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_5_ar, i_2_as_4_ar->findDirectSuccessor("1"));

        const ClosureState* i_2_as_4_ar_bs = i_2_as_4_ar->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_4_ar_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_4_ar_bs, 2));
        ASSERT_TRUE(hasMarking(i_2_as_4_ar_bs,
                               0,  0,  1,  0, 0, 1,
                               DT, DT, DT,  DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_4_ar_bs,
                               0,  1,  0,  0, 0, 1,
                               DT, 4,  0,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_4_ar_bs,    true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_4_ar_bs, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_4_ar_bs->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_4_bs = i_2_as_4->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_4_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_4_bs, 2));
        ASSERT_TRUE(hasMarking(i_2_as_4_bs,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT,  0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_4_bs,
                               0,  1,  0,  0, 1, 1,
                               DT, 4,  0,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_4_bs,    true,  true,  true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_4_bs, false,  true, false, false, true));
        ASSERT_EQ(i_2_as_5_bs, i_2_as_4_bs->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_4_ar_bs, i_2_as_4_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_2_as_3_ar = i_2_as_3->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_3_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3_ar, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar,
                               0,  0,  1,  0, 0, 0,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar,
                               0,  1,  0,  0, 0, 0,
                               DT, 3,  DT,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3_ar,    true,  true, true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3_ar, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_4_ar, i_2_as_3_ar->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_3_ar_bs = i_2_as_3_ar->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_3_ar_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3_ar_bs, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar_bs,
                               0,  0,  1,  0, 0, 1,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar_bs,
                               0,  1,  0,  0, 0, 1,
                               DT, 3,  0,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3_ar_bs,    true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3_ar_bs, false,  true, true, false, true));
        
        const ClosureState* i_2_as_3_ar_bs_1 = i_2_as_3_ar_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_3_ar_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3_ar_bs_1, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar_bs_1,
                               0,  0,  1,  0, 0, 1,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3_ar_bs_1,
                               0,  1,  0,  0, 0, 1,
                               DT, 4,  1,  DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3_ar_bs_1,    true,  true, true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3_ar_bs_1, false,  true, true, false, false));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_2_as_3_ar_bs_1->findDirectSuccessor("b?"));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_3_ar_bs_1->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_3_bs = i_2_as_3->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_3_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3_bs, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3_bs,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3_bs,
                               0,  1,  0,  0, 1, 1,
                               DT, 3,  0,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3_bs,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3_bs, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_3_ar_bs, i_2_as_3_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_2_as_3_bs_1 = i_2_as_3_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_3_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_3_bs_1, 2));
        ASSERT_TRUE(hasMarking(i_2_as_3_bs_1,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_3_bs_1,
                               0,  1,  0,  0, 1, 1,
                               DT, 4,  1,  0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_3_bs_1,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_3_bs_1, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_5_bs, i_2_as_3_bs_1->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_3_ar_bs_1, i_2_as_3_bs_1->findDirectSuccessor("a?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_2_as_3_bs_1->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_2_ar = i_2_as_2->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_2_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_ar, 1));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar,
                               0,  1,  0,  0, 0, 0,
                               DT, 2, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_ar,    true,  true,  true, true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_ar, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_3_ar, i_2_as_2_ar->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_2_ar_bs = i_2_as_2_ar->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_2_ar_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_ar_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar_bs,
                               0,  1,  0,  0, 0, 1,
                               DT, 2,  0, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_ar_bs,    true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_ar_bs, false,  true, true, false, true));
        
        const ClosureState* i_2_as_2_ar_bs_1 = i_2_as_2_ar_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_2_ar_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_ar_bs_1, 2));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar_bs_1,
                               0,  0,  1,   0, 0, 1,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar_bs_1,
                               0,  1,  0,  0, 0, 1,
                               DT, 3,  1, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_ar_bs_1,    true,  true, true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_ar_bs_1, false,  true, true, false, false));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_2_as_2_ar_bs_1->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_2_ar_bs_2 = i_2_as_2_ar_bs_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_2_ar_bs_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_ar_bs_2, 2));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar_bs_2,
                               0,  0,  1,   0, 0, 1,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_2_ar_bs_2,
                               0,  1,  0,  0, 0, 1,
                               DT, 4,  2, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_ar_bs_2,    true,  true, true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_ar_bs_2, false,  true, true, false, false));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_2_ar_bs_2->findDirectSuccessor("1"));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_2_as_2_ar_bs_2->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_2_bs = i_2_as_2->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_2_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_2_bs,
                               0,  1,  0, 0, 1, 1,
                               DT, 2,  0, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_bs,    true,  true,  true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_bs, false,  true, false, false, true));
        ASSERT_EQ(i_2_as_2_ar_bs, i_2_as_2_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_2_as_2_bs_1 = i_2_as_2_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_2_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_bs_1, 2));
        ASSERT_TRUE(hasMarking(i_2_as_2_bs_1,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_2_bs_1,
                               0,  1,  0, 0, 1, 1,
                               DT, 3,  1, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_bs_1,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_bs_1, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_2_ar_bs_1, i_2_as_2_bs_1->findDirectSuccessor("a?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_2_as_2_bs_1->findDirectSuccessor("b?"));

        const ClosureState* i_2_as_2_bs_2 = i_2_as_2_bs_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_2_bs_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_2_bs_2, 2));
        ASSERT_TRUE(hasMarking(i_2_as_2_bs_2,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_2_bs_2,
                               0,  1,  0, 0, 1, 1,
                               DT, 4,  2, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_2_bs_2,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_2_bs_2, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_5_bs, i_2_as_2_bs_2->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_2_ar_bs_2, i_2_as_2_bs_2->findDirectSuccessor("a?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_2_as_2_bs_2->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_1_ar = i_2_as_1->findDirectSuccessor("a?");
        ASSERT_FALSE(i_2_as_1_ar->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_ar, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1_ar,
                               0,  1,  0,  0, 0, 0,
                               DT, 1, DT, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_ar,    true,  true, true,  true, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_ar, false,  true, true, false, true));
        ASSERT_EQ(i_2_as_1_ar, i_2_as_ar->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_2_ar, i_2_as_1_ar->findDirectSuccessor("1"));
        
        const ClosureState* i_2_as_1_ar_bs = i_2_as_1_ar->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_1_ar_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_ar_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1_ar_bs,
                               0,  1, 0,  0, 0, 1,
                               DT, 1, 0, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_ar_bs,    true,  true, true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_ar_bs, false,  true, true, false, true));
        
        const ClosureState* i_2_as_1_ar_bs_1 = i_2_as_1_ar_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_1_ar_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_ar_bs_1, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1_ar_bs_1,
                               0,  1, 0,  0, 0, 1,
                               DT, 2, 1, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_ar_bs_1,    true,  true, true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_ar_bs_1, false,  true, true, false, false));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_2_as_1_ar_bs_1->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_1_ar_bs_2 = i_2_as_1_ar_bs_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_1_ar_bs_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_ar_bs_2, 2));
        ASSERT_TRUE(hasMarking(i_2_as_1_ar_bs_2,
                               0,  0,  1,   0, 0, 1,
                               DT, DT, DT, DT, 0));
        ASSERT_TRUE(hasMarking(i_2_as_1_ar_bs_2,
                               0,  1,  0,  0, 0, 1,
                               DT, 3,  2, DT, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_ar_bs_2,    true,  true, true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_ar_bs_2, false,  true, true, false, false));
        ASSERT_EQ(i_2_as_5_ar_bs, i_2_as_1_ar_bs_2->findDirectSuccessor("1"));
        ASSERT_EQ(i_bs_2_as_2_ar_br, i_2_as_1_ar_bs_2->findDirectSuccessor("b?"));

        
        const ClosureState* i_2_as_1_bs = i_2_as_1->findDirectSuccessor("b!");
        ASSERT_FALSE(i_2_as_1_bs->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_bs, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1_bs,
                               0,  1, 0, 0, 1, 1,
                               DT, 1, 0, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_bs,    true,  true,  true, false, true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_bs, false,  true, false, false, true));
        ASSERT_EQ(i_2_as_1_ar_bs, i_2_as_1_bs->findDirectSuccessor("a?"));
        
        const ClosureState* i_2_as_1_bs_1 = i_2_as_1_bs->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_1_bs_1->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_bs_1, 1));
        ASSERT_TRUE(hasMarking(i_2_as_1_bs_1,
                               0,  1, 0, 0, 1, 1,
                               DT, 2, 1, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_bs_1,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_bs_1, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_1_ar_bs_1, i_2_as_1_bs_1->findDirectSuccessor("a?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_2_as_1_bs_1->findDirectSuccessor("b?"));
        
        const ClosureState* i_2_as_1_bs_2 = i_2_as_1_bs_1->findDirectSuccessor("1");
        ASSERT_FALSE(i_2_as_1_bs_2->isFinal());
        ASSERT_TRUE(hasMarkings(i_2_as_1_bs_2, 2));
        ASSERT_TRUE(hasMarking(i_2_as_1_bs_2,
                               0,  0,  1,  0, 1, 1,
                               DT, DT, DT, 0, 0));
        ASSERT_TRUE(hasMarking(i_2_as_1_bs_2,
                               0,  1,  0, 0, 1, 1,
                               DT, 3,  2, 0, 0));
        ASSERT_TRUE(hasOutgoingEdges(i_2_as_1_bs_2,    true,  true,  true, false,  true));
        ASSERT_TRUE(hasEmptySuccessors(i_2_as_1_bs_2, false,  true, false, false, false));
        ASSERT_EQ(i_2_as_5_bs, i_2_as_1_bs_2->findDirectSuccessor("1"));
        ASSERT_EQ(i_2_as_1_ar_bs_2, i_2_as_1_bs_2->findDirectSuccessor("a?"));
        ASSERT_EQ(i_bs_2_as_2_br, i_2_as_1_bs_2->findDirectSuccessor("b?"));
    }
}
