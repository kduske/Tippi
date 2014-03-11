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

#ifndef __Tippi__Automaton__
#define __Tippi__Automaton__

#include "CollectionUtils.h"
#include "Exceptions.h"
#include "SharedPointer.h"
#include "StringUtils.h"
#include "GraphEdge.h"
#include "GraphNode.h"

#include <cassert>
#include <vector>

namespace Tippi {
    template <class State>
    class AutomatonEdge : public GraphEdge<State, State> {
    protected:
        String m_label;
        bool m_tau;
    protected:
        AutomatonEdge(State* source, State* target, const String& label, bool tau) :
        GraphEdge<State, State>(source, target),
        m_label(label),
        m_tau(tau) {}
    public:
        virtual ~AutomatonEdge() {}

        const String& getLabel() const {
            return m_label;
        }
        
        bool isTau() const {
            return m_tau;
        }
    };
    
    template <class Edge>
    class AutomatonState : public GraphNode<Edge, Edge> {
    protected:
        String m_name;
    protected:
        AutomatonState(const String& name) :
        m_name(name) {}
    public:
        virtual ~AutomatonState() {}
        
        const String& getName() const {
            return m_name;
        }
        
        const std::vector<typename Edge::Target*> getDirectSuccessors(const String& label) const {
            std::vector<typename Edge::Target*> result;
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->getLabel() == label)
                    result.push_back(edge->getTarget());
            }
            return result;
        }
        
        const typename Edge::Target* findDirectSuccessor(const String& label) const {
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->getLabel() == label)
                    return edge->getTarget();
            }
            return NULL;
        }
        
        const std::vector<typename Edge::Target*> getIndirectSuccessors(const String& label) const {
            std::vector<typename Edge::Target*> result;
            getIndirectSuccessors(label, result);
            return result;
        }

        const typename Edge::Target* findIndirectSuccessor(const String& label) const {
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            
            // first look for direct successors
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (!edge->isTau() && edge->getLabel() == label)
                    return edge->getTarget();
            }
            
            // now recursively look for indirect successors
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->isTau()) {
                    const typename Edge::Target* target = edge->getTarget();
                    const typename Edge::Target* successor = target->findIndirectSuccessor(label);
                    if (successor != NULL)
                        return successor;
                }
            }
            return NULL;
        }
    private:
        void getIndirectSuccessors(const String& label, std::vector<typename Edge::Target*>& result) {
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                const typename Edge::Target* successor = edge->getTarget();
                if (edge->isTau())
                    successor->getIndirectSuccessors(label, result);
                else if (edge->getLabel() == label)
                    result.push_back(successor);
            }
        }
    };
    
    template <class StateT, class EdgeT>
    class Automaton {
    public:
        typedef std::tr1::shared_ptr<Automaton<StateT,EdgeT> > Ptr;
        typedef StateT State;
        typedef EdgeT Edge;
    protected:
        typename StateT::Set m_states;
        typename EdgeT::Set m_edges;
        StateT* m_initialState;
        typename StateT::Set m_finalStates;
    protected:
        Automaton() :
        m_initialState(NULL) {}
    public:
        virtual ~Automaton() {
            SetUtils::clearAndDelete(m_states);
            SetUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        EdgeT* connectWithLabeledEdge(StateT* source, StateT* target, const String& label) {
            return connect(source, target, label, false);
        }
        
        EdgeT* connectWithTauEdge(StateT* source, StateT* target) {
            return connect(source, target, "", true);
        }

        void deleteState(StateT* state) {
            assert(state != NULL);
            typename StateT::Set::iterator it = m_states.lower_bound(state);
            assert(it != m_states.end() && SetUtils::equals(m_states, state, *it));
            
            deleteIncomingEdges(state);
            deleteOutgoingEdges(state);
            m_states.erase(it);
            
            if (m_initialState == state)
                m_initialState = NULL;
            SetUtils::remove(m_finalStates, state);
            delete state;
        }

        void disconnect(EdgeT* edge) {
            assert(edge != NULL);
            
            typename EdgeT::Set::iterator it = m_edges.find(edge);
            assert(it != m_edges.end());
            
            edge->removeFromSource();
            edge->removeFromTarget();
            m_edges.erase(it);
            delete edge;
        }

        void setInitialState(StateT* state) {
            m_initialState = state;
        }
        
        void addFinalState(StateT* state) {
            if (!state->isFinal())
                throw AutomatonException("State is not a final state: '" + state->asString() + "'");
            m_finalStates.insert(state);
        }
        
        const typename StateT::Set& getStates() const {
            return m_states;
        }
        
        const typename EdgeT::Set& getEdges() const {
            return m_edges;
        }

        StateT* getInitialState() const {
            return m_initialState;
        }
        
        const typename StateT::Set& getFinalStates() const {
            return m_finalStates;
        }
        
        template <class Other>
        bool simulates(const Other& other) const {
            typedef std::pair<const StateT*, const typename Other::State*> Pair;
            typedef std::set<Pair> Relation;
            Relation relation;
            
            return simulates(m_initialState, other.getInitialState(), relation);
        }
        
        template <class Other>
        bool weaklySimulates(const Other& other) const {
            typedef std::pair<const StateT*, const typename Other::State*> Pair;
            typedef std::set<Pair> Relation;
            Relation relation;
            
            return weaklySimulates(m_initialState, other.getInitialState(), relation);
        }
    private:
        EdgeT* connect(StateT* source, StateT* target, const String& label, const bool tau) {
            assert(source != NULL);
            assert(target != NULL);
            assert(m_states.count(source) == 1);
            assert(m_states.count(target) == 1);
            
            EdgeT* edge = new EdgeT(source, target, label, tau);
            typename EdgeT::Set::iterator it = m_edges.lower_bound(edge);
            if (it != m_edges.end() && SetUtils::equals(m_edges, edge, *it)) {
                delete edge;
                return *it;
            }
            
            m_edges.insert(it, edge);
            source->addOutgoing(edge);
            target->addIncoming(edge);
            return edge;
        }

        void deleteIncomingEdges(StateT* state) {
            const typename StateT::IncomingList& incomingEdges = state->getIncoming();
            typename StateT::IncomingList::const_iterator it, end;
            for (it = incomingEdges.begin(), end = incomingEdges.end(); it != end; ++it) {
                EdgeT* edge = *it;
                SetUtils::remove(m_edges, edge);
                edge->removeFromSource();
                delete edge;
            }
        }
        
        void deleteOutgoingEdges(StateT* state) {
            const typename StateT::OutgoingList& outgoingEdges = state->getOutgoing();
            typename StateT::OutgoingList::const_iterator it, end;
            for (it = outgoingEdges.begin(), end = outgoingEdges.end(); it != end; ++it) {
                EdgeT* edge = *it;
                SetUtils::remove(m_edges, edge);
                edge->removeFromTarget();
                delete edge;
            }
        }

        template <typename Other, typename Relation>
        bool simulates(const StateT* mine, const Other* other, Relation& relation) const {
            if (mine == NULL || other == NULL)
                return false;
            
            assert(mine != NULL);
            assert(other != NULL);
            if (!relation.insert(std::make_pair(mine, other)).second)
                return true;
            
            typename StateT::OutgoingList::const_iterator mIt, mEnd;
            typename Other::OutgoingList::const_iterator oIt, oEnd;
            
            const typename Other::OutgoingList& otherEdges = other->getOutgoing();
            
            for (oIt = otherEdges.begin(), oEnd = otherEdges.end(); oIt != oEnd; ++oIt) {
                const typename Other::Outgoing* otherEdge = *oIt;
                const Other* otherSuccessor = otherEdge->getTarget();
                const String& label = otherEdge->getLabel();
                
                const StateT* mineSuccessor = mine->findDirectSuccessor(label);
                if (mineSuccessor == NULL)
                    return false;
                if (!simulates(mineSuccessor, otherSuccessor, relation))
                    return false;
            }
            
            return true;
        }
        
        template <typename Other, typename Relation>
        bool weaklySimulates(const StateT* mine, const Other* other, Relation& relation) const {
            if (mine == NULL || other == NULL)
                return false;
            
            assert(mine != NULL);
            assert(other != NULL);
            if (!relation.insert(std::make_pair(mine, other)).second)
                return true;
            
            typename StateT::OutgoingList::const_iterator mIt, mEnd;
            typename Other::OutgoingList::const_iterator oIt, oEnd;
            
            const typename Other::OutgoingList& otherEdges = other->getOutgoing();
            
            for (oIt = otherEdges.begin(), oEnd = otherEdges.end(); oIt != oEnd; ++oIt) {
                const typename Other::Outgoing* otherEdge = *oIt;
                const Other* otherSuccessor = otherEdge->getTarget();
                if (otherEdge->isTau()) {
                    if (!weaklySimulates(mine, otherSuccessor, relation))
                        return false;
                } else {
                    const String& label = otherEdge->getLabel();
                    
                    const StateT* mineSuccessor = mine->findIndirectSuccessor(label);
                    if (mineSuccessor == NULL)
                        return false;
                    if (!weaklySimulates(mineSuccessor, otherSuccessor, relation))
                        return false;
                }
            }
            
            return true;
        }
    };
}

#endif /* defined(__Tippi__Automaton__) */
