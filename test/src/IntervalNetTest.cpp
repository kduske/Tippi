/*
 Copyright (C) 2010-2013 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "Net/IntervalNet.h"

namespace Tippi {
    namespace Interval {
        TEST(NetTest, createEmptynet) {
            Net net;
        };
        
        TEST(NetTest, createPlaceAndTransition) {
            Net net;
            ASSERT_TRUE(net.createPlace() != NULL);
            ASSERT_EQ(1u, net.places().size());
            ASSERT_TRUE(net.createTransition() != NULL);
            ASSERT_EQ(1u, net.transitions().size());
        }
        
        TEST(NetTest, connectPlaceAndTransition) {
            Net net;
            Place* p = net.createPlace();
            Transition* t = net.createTransition();
            PlaceToTransition* p2t = net.connect(p, t);
            TransitionToPlace* t2p = net.connect(t, p);
            
            ASSERT_TRUE(p2t != NULL);
            ASSERT_TRUE(p2t->getSource() == p);
            ASSERT_TRUE(p2t->getTarget() == t);
            ASSERT_TRUE(t2p != NULL);
            ASSERT_TRUE(t2p->getSource() == t);
            ASSERT_TRUE(t2p->getTarget() == p);
        }
        
        TEST(NetTest, deleteIsolatedPlace) {
            Net net;
            Place* p = net.createPlace();
            net.deletePlace(p);
            ASSERT_TRUE(net.places().empty());
        }
        
        TEST(NetTest, deleteIsolatedTransition) {
            Net net;
            Transition* t = net.createTransition();
            net.deleteTransition(t);
            ASSERT_TRUE(net.transitions().empty());
        }
        
        TEST(NetTest, deleteConnectedPlace) {
            Net net;
            Place* p = net.createPlace();
            Transition* t1 = net.createTransition();
            Transition* t2 = net.createTransition();
            net.connect(t1, p);
            net.connect(p, t2);
            
            net.deletePlace(p);
            ASSERT_TRUE(net.places().empty());
            ASSERT_TRUE(t1->getOutgoing().empty());
            ASSERT_TRUE(t2->getIncoming().empty());
        }
        
        TEST(NetTest, deleteConnectedTransition) {
            Net net;
            Place* p1 = net.createPlace();
            Place* p2 = net.createPlace();
            Transition* t = net.createTransition();
            net.connect(p1, t);
            net.connect(t, p2);
            
            net.deleteTransition(t);
            ASSERT_TRUE(net.transitions().empty());
            ASSERT_TRUE(p1->getOutgoing().empty());
            ASSERT_TRUE(p2->getIncoming().empty());
        }
    }
}
