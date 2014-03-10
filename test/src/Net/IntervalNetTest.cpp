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

#include "Net/IntervalNet.h"
#include "Exceptions.h"

namespace Tippi {
    namespace Interval {
        TEST(NetTest, createEmptynet) {
            Net net;
        };
        
        TEST(NetTest, createDuplicatePlace) {
            Net net;
            Place* p1 = net.createPlace("p1");
            ASSERT_THROW(net.createPlace("p1"), NetException);
            net.deletePlace(p1);
            ASSERT_NO_THROW(net.createPlace("p1"));
        }
        
        TEST(NetTest, createDuplicateTransition) {
            Net net;
            Transition* t1 = net.createTransition("t1", TimeInterval());
            ASSERT_THROW(net.createTransition("t1", TimeInterval()), NetException);
            net.deleteTransition(t1);
            ASSERT_NO_THROW(net.createTransition("t1", TimeInterval()));
        }
        
        TEST(NetTest, createPlaceAndTransition) {
            Net net;
            ASSERT_TRUE(net.createPlace("p1") != NULL);
            ASSERT_EQ(1u, net.getPlaces().size());
            ASSERT_TRUE(net.createTransition("t1", TimeInterval()) != NULL);
            ASSERT_EQ(1u, net.getTransitions().size());
        }
        
        TEST(NetTest, connectPlaceAndTransition) {
            Net net;
            Place* p = net.createPlace("p1");
            Transition* t = net.createTransition("t1", TimeInterval());
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
            Place* p = net.createPlace("p1");
            net.deletePlace(p);
            ASSERT_TRUE(net.getPlaces().empty());
        }
        
        TEST(NetTest, deleteIsolatedTransition) {
            Net net;
            Transition* t = net.createTransition("t1", TimeInterval());
            net.deleteTransition(t);
            ASSERT_TRUE(net.getTransitions().empty());
        }
        
        TEST(NetTest, deleteConnectedPlace) {
            Net net;
            Place* p = net.createPlace("p1");
            Transition* t1 = net.createTransition("t1", TimeInterval());
            Transition* t2 = net.createTransition("t2", TimeInterval());
            net.connect(t1, p);
            net.connect(p, t2);
            
            net.deletePlace(p);
            ASSERT_TRUE(net.getPlaces().empty());
            ASSERT_TRUE(t1->getOutgoing().empty());
            ASSERT_TRUE(t2->getIncoming().empty());
        }
        
        TEST(NetTest, deleteConnectedTransition) {
            Net net;
            Place* p1 = net.createPlace("p1");
            Place* p2 = net.createPlace("p2");
            Transition* t = net.createTransition("t1", TimeInterval());
            net.connect(p1, t);
            net.connect(t, p2);
            
            net.deleteTransition(t);
            ASSERT_TRUE(net.getTransitions().empty());
            ASSERT_TRUE(p1->getOutgoing().empty());
            ASSERT_TRUE(p2->getIncoming().empty());
        }
    }
}
