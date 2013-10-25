//
//  IntervalNet.cpp
//  Tippi
//
//  Created by Kristian Duske on 23.10.13.
//
//

#include "IntervalNet.h"
#include "CollectionUtils.h"
#include "Exceptions.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Tippi {
    namespace Interval {
        Transition::Transition(const String& name, const size_t index, const TimeInterval& interval) :
        NetNode(name, index),
        m_interval(interval) {}

        Place::Place(const String& name, const size_t index) :
        NetNode(name, index) {}

        Net::~Net() {
            VectorUtils::clearAndDelete(m_placeToTransitionArcs);
            VectorUtils::clearAndDelete(m_transitionToPlaceArcs);
        }

        Place* Net::createPlace(const String& name) {
            const size_t index = m_places.getNextIndex();
            Place* place = new Place(name, index);
            if (!m_places.insertNode(place)) {
                delete place;
                throw NetException("Net already contains a place with name '" + name + "'");
            }
            return place;
        }
        
        Transition* Net::createTransition(const String& name, const TimeInterval& interval) {
            const size_t index = m_transitions.getNextIndex();
            Transition* transition = new Transition(name, index, interval);
            if (!m_transitions.insertNode(transition)) {
                delete transition;
                throw NetException("Net already contains a transition with name '" + name + "'");
            }
            return transition;
        }
        
        PlaceToTransition* Net::connect(Place* place, Transition* transition) {
            assert(place != NULL);
            assert(transition != NULL);
            
            PlaceToTransition* a = Place::connectToTarget(place, transition);
            m_placeToTransitionArcs.push_back(a);
            return a;
        }
        
        TransitionToPlace* Net::connect(Transition* transition, Place* place) {
            assert(place != NULL);
            assert(transition != NULL);

            TransitionToPlace* a = Transition::connectToTarget(transition, place);
            m_transitionToPlaceArcs.push_back(a);
            return a;
        }

        void Net::deletePlace(Place* place) {
            assert(place != NULL);
            
            deleteIncomingArcs(place, m_transitionToPlaceArcs);
            deleteOutgoingArcs(place, m_placeToTransitionArcs);
            m_places.deleteNode(place);
        }
        
        void Net::deleteTransition(Transition* transition) {
            assert(transition != NULL);
            
            deleteIncomingArcs(transition, m_placeToTransitionArcs);
            deleteOutgoingArcs(transition, m_transitionToPlaceArcs);
            m_transitions.deleteNode(transition);
        }
        
        void Net::disconnect(PlaceToTransition* arc) {
            assert(arc != NULL);
            
            arc->removeFromSource();
            arc->removeFromTarget();
            VectorUtils::removeAndDelete(m_placeToTransitionArcs, arc);
        }
        
        void Net::disconnect(TransitionToPlace* arc) {
            assert(arc != NULL);
            
            arc->removeFromSource();
            arc->removeFromTarget();
            VectorUtils::removeAndDelete(m_transitionToPlaceArcs, arc);
        }
        
        const Place::List& Net::getPlaces() const {
            return m_places.getNodes();
        }
        
        const Transition::List& Net::getTransitions() const {
            return m_transitions.getNodes();
        }

        Place* Net::findPlace(const String& name) {
            return m_places.findNode(name);
        }
        
        Transition* Net::findTransition(const String& name) {
            return m_transitions.findNode(name);
        }
    }
}
