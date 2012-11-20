//
//  ConstructEnvironment.cpp
//  Tippi
//
//  Created by Kristian Duske on 01.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "ConstructEnvironment.h"

#include "Net.h"
#include "NetTypes.h"
#include "Place.h"
#include "Transition.h"

namespace Tippi {
    ConstructEnvironment::ConstructEnvironment(Net& net) :
    m_net(net) {}
    
    void ConstructEnvironment::operator() () {
        const PlaceList& places = m_net.places();
        PlaceList::const_iterator placeIt, placeEnd;
        for (placeIt = places.begin(), placeEnd = places.end(); placeIt != placeEnd; ++placeIt) {
            Place& place = **placeIt;
            if (place.inputPlace()) {
                Transition* transition = m_net.createTransition(place.name(), 0, Transition::Infinite, false);
                m_net.connect(transition, &place);
            } else if (place.outputPlace()) {
                Transition* transition = m_net.createTransition(place.name(), 0, 0, false);
                m_net.connect(&place, transition);
            }
        }
    }
}