//
//  Net.cpp
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "Net.h"

#include <cassert>

namespace Tippi {
    Net::~Net() {
        while (!m_placeToTransitionArcs.empty()) delete m_placeToTransitionArcs.back(), m_placeToTransitionArcs.pop_back();
        while (!m_transitionToPlaceArcs.empty()) delete m_transitionToPlaceArcs.back(), m_transitionToPlaceArcs.pop_back();
    }

    Place* Net::createPlace(const std::string& name, unsigned int bound) {
        Place* place = new Place(name, bound);
        m_places.add(place);
        return place;
    }
    
    Transition* Net::createTransition(const std::string& name, unsigned int earliestFiringTime, unsigned int latestFiringTime, bool internal) {
        Transition* transition = new Transition(name, earliestFiringTime, latestFiringTime, internal);
        m_transitions.add(transition);
        return transition;
    }
    
    PlaceToTransition* Net::connect(Place* place, Transition* transition, unsigned int multiplicity) {
        assert(place != NULL);
        assert(transition != NULL);
        assert(multiplicity > 0);
        
        PlaceToTransition* arc = new PlaceToTransition(place, transition, multiplicity);
        place->addOutgoingEdge(arc);
        transition->addIncomingEdge(arc);
        m_placeToTransitionArcs.push_back(arc);
        return arc;
    }
    
    TransitionToPlace* Net::connect(Transition* transition, Place* place, unsigned int multiplicity) {
        assert(transition != NULL);
        assert(place != NULL);
        assert(multiplicity > 0);
        
        TransitionToPlace* arc = new TransitionToPlace(transition, place, multiplicity);
        transition->addOutgoingEdge(arc);
        place->addIncomingEdge(arc);
        m_transitionToPlaceArcs.push_back(arc);
        return arc;
    }
}