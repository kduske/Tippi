//
//  IntervalTransition.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.13.
//
//

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
