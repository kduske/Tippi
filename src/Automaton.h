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
#include <list>
#include <stack>
#include <vector>

namespace Tippi {
    /**
     An edge connects two states of an automaton. An edge is directed and it can either be a 
     observable edge or unobservable, in which case its label is ignored.
     
     This class template is supposed to be derived from.
     
     @tparam State the type of the automaton states
     */
    template <class State>
    class AutomatonEdge : public GraphEdge<State, State> {
    protected:
        String m_label;
        bool m_observable;
    protected:
        /**
         Create an observable new edge that connects the given source and target states. Neither
         the source nor the target states must be NULL.
         
         @param source the source state
         @param target the target state
         @param label the label
         */
        AutomatonEdge(State* source, State* target, const String& label) :
        GraphEdge<State, State>(source, target),
        m_label(label),
        m_observable(true) {
            assert(source != NULL);
            assert(target != NULL);
        }

        /**
         Creates an unobservable edge that connects the given source and target states. Neither the
         source nor the target states must be NULL.
         
         @param source the source state
         @param target the target state
         */
        AutomatonEdge(State* source, State* target) :
        GraphEdge<State, State>(source, target),
        m_label(""),
        m_observable(false) {
            assert(source != NULL);
            assert(target != NULL);
        }
    public:
        virtual ~AutomatonEdge() {}

        /**
         Returns the label of this edge. If the edge is unobservable, the label will be empty.
         */
        const String& getLabel() const {
            return m_label;
        }
        
        /**
         Indicates whether this edge is observable.
         */
        bool isObservable() const {
            return m_observable;
        }
    };
    
    /**
     A state of an automaton. Each state has an id, which must be set after it was created, and
     each state can either be final or nonfinal.
     
     This class template is supposed to be derived from. Implementers must provide two public types
     and one static public method <tt>getKey</tt>. The <tt>Key</tt> type is used to identify 
     particular states. 
     
     The <tt>KeyCmp</tt> type is used to compare keys (and states, by virtue of the <tt>getKey</tt> 
     public method). It must provide a callable operator that takes two const pointers to state 
     instances and returns an <tt>int</tt>:
     
         int operator()(const SimpleAutomatonState* lhs, const SimpleAutomatonState* rhs) const;
     
     If lhs is considered less than rhs, the returned value must be negative. If lhs is considered
     greater than rhs, the returned value must be positive. Otherwise, the returned value must be 0.

     The static <tt>getKey</tt> method returns the key for a given state and must have a signature
     such as:
     
         static const Key& getKey(const SimpleAutomatonState* state);
     
     Together, these two types and the <tt>getKey</tt> method define a weak ordering of the
     automaton states.
     
     @tparam Edge the type of the edges of the automaton
     */
    template <class Sub, class Edge>
    class AutomatonState : public GraphNode<Edge, Edge> {
    protected:
        size_t m_id;
        bool m_final;
    protected:
        /**
         Creates a new, nonfinal state.
         */
        AutomatonState() :
        m_id(0),
        m_final(false) {}
    public:
        virtual ~AutomatonState() {}

        /**
         Returns the id of this state.
         */
        size_t getId() const {
            assert(m_id > 0);
            return m_id;
        }
        
        /**
         Sets the id of this state.
         
         @param i_id the new id
         */
        void setId(const size_t i_id) {
            assert(i_id > 0);
            m_id = i_id;
        }
        
        /**
         Indicates whether this state is final.
         */
        bool isFinal() const {
            return m_final;
        }
        
        /**
         Specifies whether this state is final
         
         @param final whether the state is final or not
         */
        void setFinal(const bool final) {
            m_final = final;
        }
        
        /**
         Indicates whether this state has an observable incoming edge with the given label.
         
         @param label the label of the incoming edge
         */
        bool hasIncomingEdge(const String& label) const {
            return findDirectPredecessor(label) != NULL;
        }
        
        /**
         Indicates whether this state has an observable outgoing edge with the given label.
         
         @param label the laben of the outgoing edge
         */
        bool hasOutgoingEdge(const String& label) const {
            return findDirectSuccessor(label) != NULL;
        }
        
        /**
         Returns a vector of all states which have an observable outgoing edge ending in this state
         and having the given label.
         
         @param label the label of the incoming edges
         @return a possibly empty vector of states
         */
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
        
        /**
         Returns a state that has an observable outgoing edge ending in this state and having the 
         given label. If no such state exists, this method returns NULL. If more than one such 
         states exist, any of these states can be returned.
         
         @param label the label of the incoming edge
         @return a state or NULL
         */
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
        
        /**
         Returns a vector of all states which have an observable incoming edge originating in this 
         state and having the given label.
         
         @param label the label of the outgoing edges
         @return a possibly empty vector of states
         */
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
        
        /**
         Returns a state that has an observable incoming edge originating in this state and having 
         the given label. If no such state exists, this method returns NULL. If more than one such 
         states exist, this method returns one of these states.
         
         @param label the label of the outgoing edge
         @return a state or NULL
         */
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
        
        /**
         Returns a vector of all states reachable from this state with only unobservable edges 
         followed by one edge with the given label.
         
         @param label the label of the last edge
         @return a possibly empty vector of states
         */
        const std::vector<typename Edge::Target*> getIndirectSuccessors(const String& label) const {
            std::vector<typename Edge::Target*> result;
            getIndirectSuccessors(label, result);
            return result;
        }

        /**
         Returns a state which is reachable from this state with only unobservable edges followed
         by one edge with the given label. If no such state exists, this method returns NULL. If
         more than one such states exist, this method returns one of these states.
         
         @param label the label of the last edge
         @return a state or NULL
         */
        const typename Edge::Target* findIndirectSuccessor(const String& label) const {
            const typename GraphNode<Edge, Edge>::OutgoingList& edges = GraphNode<Edge, Edge>::getOutgoing();
            typename GraphNode<Edge, Edge>::OutgoingList::const_iterator it, end;
            
            // first look for direct successors
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->isObservable() && edge->getLabel() == label)
                    return edge->getTarget();
            }
            
            // now recursively look for indirect successors
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (!edge->isObservable()) {
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
    
    template <class StateT>
    class Component {
    public:
        typedef std::list<Component> List;
        typedef std::set<StateT*> StateSet;
    private:
        StateSet m_states;
    public:
        const StateSet& getStates() const {
            return m_states;
        }
        
        void addState(StateT* state) {
            m_states.insert(state);
        }
        
        bool containsFinalState() const {
            typename StateSet::const_iterator sIt, sEnd;
            for (sIt = m_states.begin(), sEnd = m_states.end(); sIt != sEnd; ++sIt) {
                const StateT* state = *sIt;
                if (state->isFinal())
                    return true;
            }
            return false;
        }
        
        bool hasExit() const {
            typename StateSet::const_iterator sIt, sEnd;
            for (sIt = m_states.begin(), sEnd = m_states.end(); sIt != sEnd; ++sIt) {
                const StateT* state = *sIt;
                typename StateT::Outgoing::List& outgoing = state->getOutgoing();
                typename StateT::Outgoing::List::const_iterator eIt, eEnd;
                for (eIt = outgoing.begin(), eEnd = outgoing.end(); eIt != eEnd; ++eIt) {
                    const typename StateT::Outgoing* edge = *eIt;
                    const StateT* target = edge->getTarget();
                    if (m_states.count(target) > 0)
                        return true;
                }
            }
            
            return false;
        }
    };
    
    template <class StateT>
    class ComputeComponents {
    private:
        typedef Component<StateT> ComponentT;
    public:
        typedef typename ComponentT::List ComponentList;
    private:
        typedef size_t Index;
        
        struct StateInfo {
            Index index;
            Index lowLink;
            bool onStack;
            
            StateInfo() : index(0), lowLink(0), onStack(false) {}
            StateInfo(const Index i_index) : index(i_index), lowLink(i_index), onStack(true) {}
            
            bool isRootNode() const {
                return lowLink == index;
            }
            
            void updateLowLink(const Index i_index) {
                lowLink = std::min(lowLink, i_index);
            }
        };
        
        typedef std::map<StateT*, StateInfo> InfoMap;
        InfoMap m_infos;
        
        typedef std::stack<StateT*> StateStack;
        StateStack m_stack;
        
        Index m_index;
        ComponentList m_components;
    public:
        ComputeComponents(StateT* initialState) :
        m_index(0) {
            visitState(initialState);
        }

        const ComponentList& getComponents() const {
            return m_components;
        }
    private:
        StateInfo& visitState(StateT* state) {
            assert(m_infos.count(state) == 0);
            
            typename InfoMap::iterator sIt = m_infos.insert(std::make_pair(state, StateInfo(m_index++))).first;
            StateInfo& stateInfo = sIt->second;
            m_stack.push(state);
            
            typedef typename StateT::Outgoing::List OutList;
            const OutList& edges = state->getOutgoing();
            typename OutList::const_iterator eIt, eEnd;
            for (eIt = edges.begin(), eEnd = edges.end(); eIt != eEnd; ++eIt) {
                typename StateT::Outgoing* edge = *eIt;
                
                typename InfoMap::iterator iIt = m_infos.find(edge->getTarget());
                if (iIt == m_infos.end()) {
                    const StateInfo& targetInfo = visitState(edge->getTarget());
                    stateInfo.updateLowLink(targetInfo.lowLink);
                } else {
                    const StateInfo& targetInfo = iIt->second;
                    if (targetInfo.onStack)
                        stateInfo.updateLowLink(targetInfo.index);
                }
            }
            
            if (stateInfo.isRootNode()) {
                ComponentT component;
                StateT* top;
                do {
                    top = m_stack.top(); m_stack.pop();
                    m_infos[top].onStack = false;
                    component.addState(top);
                } while (top != state);
                
                m_components.push_back(component);
            }
            
            return stateInfo;
        }
    };
    
    /**
     An automaton consisting of states and directed edges. Each automaton can have one initial state
     and zero or more final states.
     
     This class template is supposed to be derived from.
     
     @tparam StateT the type of the automaton states, must be derived from AutomatonState
     @tparam EdgeT the type of the automaton edges, must be derived from AutomatonEdge
     */
    template <class StateT, class EdgeT>
    class Automaton {
    private:
        /**
         Implements a weak total order for states by virtue of the StateT::Key type,
         the StateT::KeyCmp type and the static StateT::getKey method.
         */
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
        /**
         Implements a weak total order for edges by virtue of the StateT::Key type,
         the StateT::KeyCmp type and the static StateT::getKey method. If the source and target
         states of two edges are considered equal, the label is used as the order criterion.
         */
        struct EdgeLess {
            typename StateT::KeyCmp m_stateCmp;
            
            bool operator() (const EdgeT* lhs, const EdgeT* rhs) const {
                assert(lhs != NULL);
                assert(rhs != NULL);

                const int sourceResult = m_stateCmp(StateT::getKey(lhs->getSource()), StateT::getKey(rhs->getSource()));
                if (sourceResult < 0)
                    return true;
                if (sourceResult > 0)
                    return false;
                
                const int targetResult = m_stateCmp(StateT::getKey(lhs->getTarget()), StateT::getKey(rhs->getTarget()));
                if (targetResult < 0)
                    return true;
                if (targetResult > 0)
                    return false;
                return lhs->getLabel().compare(rhs->getLabel()) < 0;
            }
        };
    public:
        typedef StateT State;
        typedef EdgeT Edge;
        typedef Component<State> Component;
        typedef typename Component::List ComponentList;
        
        typedef std::list<StateT*> StateList;
        typedef std::set<StateT*, StateLess> StateSet;
        typedef std::set<EdgeT*, EdgeLess> EdgeSet;
    private:
        StateSet m_states;
        EdgeSet m_edges;
        StateT* m_initialState;
        StateSet m_finalStates;
        
        size_t m_nextId;
    protected:
        /**
         Creates a new automaton.
         */
        Automaton() :
        m_initialState(NULL),
        m_nextId(1) {}
    public:
        virtual ~Automaton() {
            CollectionUtils::clearAndDelete(m_states);
            CollectionUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        /**
         Returns the maximum state id.
         */
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
        
        EdgeT* connectWithObservableEdge(StateT* source, StateT* target, const String& label) {
            return connect(new EdgeT(source, target, label));
        }
        
        template <typename A1>
        EdgeT* connectWithObservableEdge(StateT* source, StateT* target, const String& label, const A1& a1) {
            return connect(new EdgeT(source, target, label, a1));
        }
        
        template <typename A1, typename A2>
        EdgeT* connectWithObservableEdge(StateT* source, StateT* target, const String& label, const A1& a1, const A2& a2) {
            return connect(new EdgeT(source, target, label, a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        EdgeT* connectWithObservableEdge(StateT* source, StateT* target, const String& label, const A1& a1, const A2& a2, const A3& a3) {
            return connect(new EdgeT(source, target, label, a1, a2, a3));
        }
        
        EdgeT* connectWithUnobservableEdge(StateT* source, StateT* target) {
            return connect(new EdgeT(source, target));
        }
        
        template <typename A1>
        EdgeT* connectWithUnobservableEdge(StateT* source, StateT* target, const A1& a1) {
            return connect(new EdgeT(source, target, a1));
        }
        
        template <typename A1, typename A2>
        EdgeT* connectWithUnobservableEdge(StateT* source, StateT* target, const A1& a1, const A2& a2) {
            return connect(new EdgeT(source, target, a1, a2));
        }
        
        template <typename A1, typename A2, typename A3>
        EdgeT* connectWithUnobservableEdge(StateT* source, StateT* target, const A1& a1, const A2& a2, const A3& a3) {
            return connect(new EdgeT(source, target, a1, a2, a3));
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
        
        ComponentList computeComponents() const {
            ComputeComponents<State> compute(getInitialState());
            return compute.getComponents();
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
    protected:
        void replaceState(StateT* oldState, StateT* newState) {
            assert(oldState != NULL);
            assert(newState != NULL);
            assert(oldState != newState);
            
            assert(m_states.count(oldState) == 1);
            m_states.erase(oldState);

            SetUtils::remove(m_edges, oldState->getIncoming().begin(), oldState->getIncoming().end());
            SetUtils::remove(m_edges, oldState->getOutgoing().begin(), oldState->getOutgoing().end());
            
            oldState->replaceAsSource(newState);
            oldState->replaceAsTarget(newState);
            
            m_edges.insert(newState->getIncoming().begin(), newState->getIncoming().end());
            m_edges.insert(newState->getOutgoing().begin(), newState->getOutgoing().end());
            
            if (m_initialState == oldState)
                m_initialState = newState;
            
            const typename StateSet::iterator it = m_finalStates.find(oldState);
            if (it != m_finalStates.end()) {
                m_finalStates.erase(oldState);
                m_finalStates.insert(newState);
            }
            
            delete oldState;
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
                if (!otherEdge->isObservable()) {
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
