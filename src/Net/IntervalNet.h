//
//  IntervalTransition.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.13.
//
//

#ifndef __Tippi__IntervalNet__
#define __Tippi__IntervalNet__

#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"

namespace Tippi {
    namespace Interval {
        class Transition;
        class Place;

        typedef GraphEdge<Transition, Place> TransitionToPlace;
        typedef GraphEdge<Place, Transition> PlaceToTransition;

        class Transition : public GraphNode<PlaceToTransition, TransitionToPlace> {
        public:
        };
        
        class Place : public GraphNode<TransitionToPlace, PlaceToTransition> {
        public:
        };
        
        class Net {
        private:
            Place::List m_places;
            Transition::List m_transitions;
            PlaceToTransition::List m_placeToTransitionArcs;
            TransitionToPlace::List m_transitionToPlaceArcs;
        public:
            ~Net();
            
            Place* createPlace();
            Transition* createTransition();
            PlaceToTransition* connect(Place* place, Transition* transition);
            TransitionToPlace* connect(Transition* transition, Place* place);

            void deletePlace(Place* place);
            void deleteTransition(Transition* transition);
            void disconnect(PlaceToTransition* arc);
            void disconnect(TransitionToPlace* arc);
            
            const Place::List& places() const;
            const Transition::List& transitions() const;
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
