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

#include "IntervalNet.h"
#include "CollectionUtils.h"
#include "Exceptions.h"
#include "IntervalNetState.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Tippi {
    namespace Interval {
        Transition::Transition(const String& name, const size_t index, const TimeInterval& interval) :
        NetNode(name, index),
        m_interval(interval),
        m_label(name) {}

        const TimeInterval& Transition::getInterval() const {
            return m_interval;
        }

        const String& Transition::getLabel() const {
            return m_label;
        }
        
        void Transition::setLabel(const String& label) {
            m_label = label;
        }

        bool Transition::isInputSend() const {
            const TransitionToPlace::List& outgoing = getOutgoing();
            TransitionToPlace::List::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const TransitionToPlace* arc = *it;
                const Place* place = arc->getTarget();
                if (place->isInputPlace())
                    return true;
            }
            return false;
        }
        
        bool Transition::isInputRead() const {
            const PlaceToTransition::List& incoming = getIncoming();
            PlaceToTransition::List::const_iterator it, end;
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                const PlaceToTransition* arc = *it;
                const Place* place = arc->getSource();
                if (place->isInputPlace())
                    return true;
            }
            return false;
        }
        
        bool Transition::isOutputSend() const {
            const TransitionToPlace::List& outgoing = getOutgoing();
            TransitionToPlace::List::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                const TransitionToPlace* arc = *it;
                const Place* place = arc->getTarget();
                if (place->isOutputPlace())
                    return true;
            }
            return false;
        }
        
        bool Transition::isOutputRead() const {
            const PlaceToTransition::List& incoming = getIncoming();
            PlaceToTransition::List::const_iterator it, end;
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                const PlaceToTransition* arc = *it;
                const Place* place = arc->getSource();
                if (place->isOutputPlace())
                    return true;
            }
            return false;
        }

        Place::Place(const String& name, const size_t index, const size_t bound) :
        NetNode(name, index),
        m_bound(bound),
        m_inputPlace(false),
        m_outputPlace(false) {}

        size_t Place::getBound() const {
            return m_bound;
        }

        bool Place::isInputPlace() const {
            return m_inputPlace;
        }
        
        bool Place::isOutputPlace() const {
            return m_outputPlace;
        }
        
        void Place::setInputPlace(const bool inputPlace) {
            m_inputPlace = inputPlace;
        }
        
        void Place::setOutputPlace(const bool outputPlace) {
            m_outputPlace = outputPlace;
        }

        Net::~Net() {
            VectorUtils::clearAndDelete(m_placeToTransitionArcs);
            VectorUtils::clearAndDelete(m_transitionToPlaceArcs);
        }

        Place* Net::createPlace(const String& name, const size_t bound) {
            const size_t index = m_places.getNextIndex();
            Place* place = new Place(name, index, bound);
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
        
        void Net::setInitialMarking(const Marking& marking) {
            m_initialMarking = marking;
        }

        void Net::addFinalMarking(const Marking& marking) {
            VectorUtils::setInsert(m_finalMarkings, marking);
        }

        const Place::List& Net::getPlaces() const {
            return m_places.getNodes();
        }
        
        const Transition::List& Net::getTransitions() const {
            return m_transitions.getNodes();
        }

        const Place* Net::findPlace(const String& name) const {
            return m_places.findNode(name);
        }
        
        const Transition* Net::findTransition(const String& name) const {
            return m_transitions.findNode(name);
        }

        Place* Net::findPlace(const String& name) {
            return m_places.findNode(name);
        }
        
        Transition* Net::findTransition(const String& name) {
            return m_transitions.findNode(name);
        }
        
        const Marking& Net::getInitialMarking() const {
            return m_initialMarking;
        }

        const Marking::List& Net::getFinalMarkings() const {
            return m_finalMarkings;
        }

        bool Net::isFinalMarking(const Marking& marking) const {
            return VectorUtils::setContains(m_finalMarkings, marking);
        }

        bool Net::isBounded(const Marking& marking) const {
            const Place::List& places = getPlaces();
            Place::List::const_iterator it, end;
            for (it = places.begin(), end = places.end(); it != end; ++it) {
                const Place* place = *it;
                if (marking[place] > place->getBound())
                    return false;
            }
            return true;
        }

        bool Net::isClosed() const {
            const Place::List& places = getPlaces();
            Place::List::const_iterator it, end;
            for (it = places.begin(), end = places.end(); it != end; ++it) {
                const Place* place = *it;
                if (place->isInputPlace() || place->isOutputPlace())
                    return false;
            }
            return true;
        }
    }
}
