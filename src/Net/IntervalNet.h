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

#ifndef __Tippi__IntervalNet__
#define __Tippi__IntervalNet__

#include "StringUtils.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"
#include "Net/NetNode.h"
#include "Net/TimeInterval.h"

#include <exception>
#include <map>

namespace Tippi {
    namespace Interval {
        class Transition;
        class Place;

        typedef GraphEdge<Transition, Place> TransitionToPlace;
        typedef GraphEdge<Place, Transition> PlaceToTransition;

        class Transition : public GraphNode<PlaceToTransition, TransitionToPlace>, public NetNode {
        public:
            typedef std::vector<Transition*> List;
        private:
            TimeInterval m_interval;
        public:
            Transition(const String& name, const size_t index, const TimeInterval& interval);
        };
        
        class Place : public GraphNode<TransitionToPlace, PlaceToTransition>, public NetNode {
        public:
            typedef std::vector<Place*> List;
        public:
            Place(const String& name, const size_t index);
        };
        
        class Net {
        private:
            NetNodeStore<Place> m_places;
            NetNodeStore<Transition> m_transitions;
            PlaceToTransition::List m_placeToTransitionArcs;
            TransitionToPlace::List m_transitionToPlaceArcs;
        public:
            ~Net();
            
            Place* createPlace(const String& name);
            Transition* createTransition(const String& name, const TimeInterval& interval);
            PlaceToTransition* connect(Place* place, Transition* transition);
            TransitionToPlace* connect(Transition* transition, Place* place);

            void deletePlace(Place* place);
            void deleteTransition(Transition* transition);
            void disconnect(PlaceToTransition* arc);
            void disconnect(TransitionToPlace* arc);
            
            const Place::List& getPlaces() const;
            const Transition::List& getTransitions() const;
            Place* findPlace(const String& name);
            Transition* findTransition(const String& name);
        private:
            template <class Node, class ArcList>
            void deleteIncomingArcs(Node* node, ArcList& arcs) {
                const typename Node::IncomingList& incomingArcs = node->getIncoming();
                typename Node::IncomingList::const_iterator it, end;
                for (it = incomingArcs.begin(), end = incomingArcs.end(); it != end; ++it) {
                    typename Node::Incoming* arc = *it;
                    arc->removeFromSource();
                    VectorUtils::removeAndDelete(arcs, arc);
                }
            }
            
            template <class Node, class ArcList>
            void deleteOutgoingArcs(Node* node, ArcList& arcs) {
                const typename Node::OutgoingList& outgoingArcs = node->getOutgoing();
                typename Node::OutgoingList::const_iterator it, end;
                for (it = outgoingArcs.begin(), end = outgoingArcs.end(); it != end; ++it) {
                    typename Node::Outgoing* arc = *it;
                    arc->removeFromTarget();
                    VectorUtils::removeAndDelete(arcs, arc);
                }
            }
            
        };
    }
}

#endif /* defined(__Tippi__IntervalTransition__) */
