//
//  IntervalNet.cpp
//  Tippi
//
//  Created by Kristian Duske on 23.10.13.
//
//

#include "IntervalNet.h"
#include "CollectionUtils.h"

namespace Tippi {
    namespace Interval {
        Net::~Net() {
            VectorUtils::clearAndDelete(m_placeToTransitionArcs);
            VectorUtils::clearAndDelete(m_transitionToPlaceArcs);
            VectorUtils::clearAndDelete(m_places);
            VectorUtils::clearAndDelete(m_transitions);
        }

        Place* Net::createPlace() {
            Place* p = new Place();
            m_places.push_back(p);
            return p;
        }
        
        Transition* Net::createTransition() {
            Transition* t = new Transition();
            m_transitions.push_back(t);
            return t;
        }
        
        PlaceToTransition* Net::connect(Place* place, Transition* transition) {
            PlaceToTransition* a = Place::connectToTarget(place, transition);
            m_placeToTransitionArcs.push_back(a);
            return a;
        }
        
        TransitionToPlace* Net::connect(Transition* transition, Place* place) {
            TransitionToPlace* a = Transition::connectToTarget(transition, place);
            m_transitionToPlaceArcs.push_back(a);
            return a;
        }

        void Net::deletePlace(Place* place) {
            deleteIncomingArcs(place, m_transitionToPlaceArcs);
            deleteOutgoingArcs(place, m_placeToTransitionArcs);
            VectorUtils::removeAndDelete(m_places, place);
        }
        
        void Net::deleteTransition(Transition* transition) {
            deleteIncomingArcs(transition, m_placeToTransitionArcs);
            deleteOutgoingArcs(transition, m_transitionToPlaceArcs);
            VectorUtils::removeAndDelete(m_transitions, transition);
        }
        
        void Net::disconnect(PlaceToTransition* arc) {
            arc->removeFromSource();
            arc->removeFromTarget();
            VectorUtils::removeAndDelete(m_placeToTransitionArcs, arc);
        }
        
        void Net::disconnect(TransitionToPlace* arc) {
            arc->removeFromSource();
            arc->removeFromTarget();
            VectorUtils::removeAndDelete(m_transitionToPlaceArcs, arc);
        }
        
        const Place::List& Net::places() const {
            return m_places;
        }
        
        const Transition::List& Net::transitions() const {
            return m_transitions;
        }
    }
}
