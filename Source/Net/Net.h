//
//  Net.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_Net_h
#define Tippi_Net_h

#include "NetTypes.h"
#include "Arc.h"
#include "NetNodeStore.h"
#include "Place.h"
#include "PlaceMarking.h"
#include "Transition.h"

namespace Tippi {
    class Net {
    protected:
        NetNodeStore<Place> m_places;
        NetNodeStore<Transition> m_transitions;
        PlaceToTransitionArcList m_placeToTransitionArcs;
        TransitionToPlaceArcList m_transitionToPlaceArcs;
        
        PlaceMarking m_initialPlaceMarking;
        PlaceMarkingSet m_finalPlaceMarkings;
    public:
        ~Net();
        
        Place* createPlace(const std::string& name, unsigned int bound);

        inline const PlaceList& places() const {
            return m_places.nodes();
        }
        
        inline Place* place(const std::string& name) {
            return m_places[name];
        }
        
        Transition* createTransition(const std::string& name, unsigned int earliestFiringTime = 0, unsigned int latestFiringTime = Transition::Infinite, bool internal = true);
        
        inline const TransitionList& transitions() const {
            return m_transitions.nodes();
        }
        
        inline Transition* transition(const std::string& name) {
            return m_transitions[name];
        }
        
        PlaceToTransition* connect(Place* place, Transition* transition, unsigned int multiplicity = 1);
        TransitionToPlace* connect(Transition* transition, Place* place, unsigned int multiplicity = 1);
        
        inline const PlaceMarking& initialPlaceMarking() const {
            return m_initialPlaceMarking;
        }
        
        inline void setInitialPlaceMarking(const PlaceMarking& initialPlaceMarking) {
            m_initialPlaceMarking = initialPlaceMarking;
        }
        
        inline void addFinalPlaceMarking(const PlaceMarking& finalPlaceMarking) {
            m_finalPlaceMarkings.insert(finalPlaceMarking);
        }
        
        inline bool isFinalPlaceMarking(const PlaceMarking& placeMarking) const {
            return m_finalPlaceMarkings.count(placeMarking) > 0;
        }
    };
}

#endif
