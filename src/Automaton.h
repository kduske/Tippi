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
        m_tau(tau) {
            assert(source != NULL);
            assert(target != NULL);
            assert(!tau || label.empty());
        }
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
        size_t m_id;
        bool m_final;
    protected:
        AutomatonState() :
        m_id(0),
        m_final(false) {}
    public:
        virtual ~AutomatonState() {}

        size_t getId() const {
            assert(m_id > 0);
            return m_id;
        }
        
        void setId(const size_t i_id) {
            assert(i_id > 0);
            m_id = i_id;
        }
        
        bool isFinal() const {
            return m_final;
        }
        
        void setFinal(const bool final) {
            m_final = final;
        }

        bool hasIncomingEdge(const String& label) const {
            return findDirectPredecessor(label) != NULL;
        }
        
        bool hasOutgoingEdge(const String& label) const {
            return findDirectSuccessor(label) != NULL;
        }
        
        const std::vector<typename Edge::Source*> getDirectPredecessors(const String& label) const {
            std::vector<typename Edge::Target*> result;
            const typename GraphNode<Edge, Edge>::IncomingList& edges = GraphNode<Edge, Edge>::getIncoming();
            typename GraphNode<Edge, Edge>::IncomingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                Edge* edge = *it;
                if (edge->getLabel() == label)
                    result.push_back(edge->getSource());
            }
            return result;
        }
        
        const typename Edge::Source* findDirectPredecessor(const String& label) const {
            const typename GraphNode<Edge, Edge>::IncomingList& edges = GraphNode<Edge, Edge>::getIncoming();
            typename GraphNode<Edge, Edge>::IncomingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->getLabel() == label)
                    return edge->getSource();
            }
            return NULL;
        }
        
        const std::vector<typename Edge::Target*> getDirectSuccessors(const String& label) const {
            std::vector<typename Edge::Target*> result;
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                Edge* edge = *it;
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
    private:
        struct StateLess {
            typename StateT::KeyCmp m_cmp;
            
            bool operator() (const StateT* lhs, const StateT* rhs) const {
                assert(lhs != NULL);
                assert(rhs != NULL);
                const int result = m_cmp(StateT::getKey(lhs), StateT::getKey(rhs));
                return result < 0;
            }
            
            bool operator() (const typename StateT::Key& lhs, const StateT* rhs) const {
                assert(rhs != NULL);
                return m_cmp(lhs, StateT::getKey(rhs)) < 0;
            }
            
            bool operator() (const StateT* lhs, const typename StateT::Key& rhs) const {
                assert(lhs != NULL);
                return m_cmp(StateT::getKey(lhs), rhs) < 0;
            }
            
            bool operator() (const typename StateT::Key& lhs, const typename StateT::Key& rhs) const {
                return m_cmp(lhs, rhs) < 0;
            }
        };
        struct EdgeLess {
            typename StateT::KeyCmp m_stateCmp;
            
            bool operator() (const EdgeT* lhs, const EdgeT* rhs) const {
                assert(lhs != NULL);
                assert(rhs != NULL);

                const int sourceResult = m_stateCmp(StateT::getKey(lhs->getSource()), StateT::getKey(rhs->getSource()));
                if (sourceResult < 0)
                    return -1;
                if (sourceResult > 0)
                    return 1;
                
                const int targetResult = m_stateCmp(StateT::getKey(lhs->getTarget()), StateT::getKey(rhs->getTarget()));
                if (targetResult < 0)
                    return -1;
                if (targetResult > 0)
                    return 1;
                return lhs->getLabel().compare(rhs->getLabel());
            }
        };
    public:
        typedef StateT State;
        typedef EdgeT Edge;
        typedef std::set<StateT*, StateLess> StateSet;
        typedef std::set<EdgeT*, EdgeLess> EdgeSet;
    private:
        StateSet m_states;
        EdgeSet m_edges;
        StateT* m_initialState;
        StateSet m_finalStates;
        
        size_t m_nextId;
    protected:
        Automaton() :
        m_initialState(NULL),
        m_nextId(1) {}
    public:
        virtual ~Automaton() {
            SetUtils::clearAndDelete(m_states);
            SetUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        size_t getMaxId() const {
            return m_nextId - 1;
        }
        
        template <typename A1>
        StateT* findState(const A1& a1) const {
            State state(a1);
            return findState(state);
        }
        
        template <typename A1, typename A2>
        StateT* findState(const A1& a1, const A2& a2) const {
            State state(a1, a2);
            return findState(state);
        }
        
        template <typename A1, typename A2, typename A3>
        StateT* findState(const A1& a1, const A2& a2, const A3& a3) const {
            State state(a1, a2, a3);
            return findState(state);
        }

        StateT* createState() {
            return addState(new State());
        }
        
        template <typename A1>
        StateT* createState(const A1& a1) {
            return addState(new State(a1));
        }
        
        template <typename A1, typename A2>
        StateT* createState(const A1& a1, const A2& a2) {
            return addState(new State(a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        StateT* createState(const A1& a1, const A2& a2, const A3& a3) {
            return addState(new State(a1, a3));
        }
        
        template <typename A1>
        std::pair<StateT*, bool> findOrCreateState(const A1& a1) {
            return findOrAddState(new State(a1));
        }
        
        template <typename A1, typename A2>
        std::pair<StateT*, bool> findOrCreateState(const A1& a1, const A2& a2) {
            return findOrAddState(new State(a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        std::pair<StateT*, bool> findOrCreateState(const A1& a1, const A2& a2, const A3& a3) {
            return findOrAddState(new State(a1, a3));
        }
        
        void deleteState(StateT* state) {
            assert(state != NULL);
            typename StateSet::iterator it = m_states.lower_bound(state);
            assert(it != m_states.end() && SetUtils::equals(m_states, state, *it));
            
            deleteIncomingEdges(state);
            deleteOutgoingEdges(state);
            m_states.erase(it);
            
            if (m_initialState == state)
                m_initialState = NULL;
            SetUtils::remove(m_finalStates, state);
            delete state;
        }
        
        template <typename I>
        void deleteStates(I cur, I end) {
            while (cur != end) {
                deleteState(*cur);
                ++cur;
            }
        }
        
        EdgeT* connectWithLabeledEdge(StateT* source, StateT* target, const String& label) {
            return connect(new EdgeT(source, target, label, false));
        }
        
        template <typename A1>
        EdgeT* connectWithLabeledEdge(StateT* source, StateT* target, const String& label, const A1& a1) {
            return connect(new EdgeT(source, target, label, false, a1));
        }
        
        template <typename A1, typename A2>
        EdgeT* connectWithLabeledEdge(StateT* source, StateT* target, const String& label, const A1& a1, const A2& a2) {
            return connect(new EdgeT(source, target, label, false, a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        EdgeT* connectWithLabeledEdge(StateT* source, StateT* target, const String& label, const A1& a1, const A2& a2, const A3& a3) {
            return connect(new EdgeT(source, target, label, false, a1, a2, a3));
        }
        
        EdgeT* connectWithTauEdge(StateT* source, StateT* target) {
            return connect(new EdgeT(source, target, "", true));
        }
        
        template <typename A1>
        EdgeT* connectWithTauEdge(StateT* source, StateT* target, const A1& a1) {
            return connect(new EdgeT(source, target, "", true, a1));
        }
        
        template <typename A1, typename A2>
        EdgeT* connectWithTauEdge(StateT* source, StateT* target, const A1& a1, const A2& a2) {
            return connect(new EdgeT(source, target, "", true, a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        EdgeT* connectWithTauEdge(StateT* source, StateT* target, const A1& a1, const A2& a2, const A3& a3) {
            return connect(new EdgeT(source, target, "", true, a1, a2, a3));
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
                throw AutomatonException("Cannot add nonfinal state to set of final states");
            m_finalStates.insert(state);
        }
        
        const StateSet& getStates() const {
            return m_states;
        }
        
        const EdgeSet& getEdges() const {
            return m_edges;
        }

        StateT* getInitialState() const {
            return m_initialState;
        }
        
        const StateSet& getFinalStates() const {
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
        StateT* findState(StateT& state) const {
            typename StateSet::const_iterator it = m_states.lower_bound(&state);
            if (it == m_states.end() || !SetUtils::equals(m_states, &state, *it))
                return NULL;
            return *it;
        }
        
        StateT* addState(StateT* state) {
            assert(state != NULL);
            
            if (!m_states.insert(state).second) {
                delete state;
                throw AutomatonException("Cannot add the same state twice");
            }
            setStateId(state);
            stateWasAdded(state);
            return state;
        }
        
        std::pair<StateT*, bool> findOrAddState(StateT* state) {
            assert(state != NULL);
            
            typename StateSet::iterator it = m_states.lower_bound(state);
            if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
                delete state;
                return std::make_pair(*it, false);
            }
            
            m_states.insert(it, state);
            setStateId(state);
            stateWasAdded(state);
            return std::make_pair(state, true);
        }

        void setStateId(StateT* state) {
            state->setId(m_nextId++);
        }
        
        virtual void stateWasAdded(StateT* state) {}
        
        EdgeT* connect(EdgeT* edge) {
            assert(m_states.count(edge->getSource()) == 1);
            assert(m_states.count(edge->getTarget()) == 1);

            typename EdgeSet::iterator it = m_edges.lower_bound(edge);
            if (it != m_edges.end() && SetUtils::equals(m_edges, edge, *it)) {
                delete edge;
                return *it;
            }
            
            edge->getSource()->addOutgoing(edge);
            edge->getTarget()->addIncoming(edge);
            m_edges.insert(it, edge);
            
            edgeWasAdded(edge);
            return edge;
        }
        
        virtual void edgeWasAdded(EdgeT* edge) {}

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
