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

#include "Behavior.h"

#include "CollectionUtils.h"
#include "Exceptions.h"

#include <cassert>

namespace Tippi {
    namespace Behavior {
        Edge::Edge(State* source, State* target, const String& label) :
        GraphEdge(source, target),
        m_label(label) {}
        
        bool Edge::operator<(const Edge& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool Edge::operator<(const Edge* rhs) const {
            return compare(*rhs) < 0;
        }

        int Edge::compare(const Edge& rhs) const {
            const int sourceResult = getSource()->compare(*rhs.getSource());
            if (sourceResult < 0)
                return -1;
            if (sourceResult > 0)
                return 1;
            const int targetResult = getTarget()->compare(*rhs.getTarget());
            if (targetResult < 0)
                return -1;
            if (targetResult > 0)
                return 1;
            return m_label.compare(rhs.m_label);
        }

        const String& Edge::getLabel() const {
            return m_label;
        }

        State::State(const Interval::NetState& netState) :
        m_netState(netState),
        m_final(false),
        m_boundViolation(false) {}
        
        State::State() :
        m_netState(0, 0),
        m_final(false),
        m_boundViolation(true) {}

        bool State::operator<(const State& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool State::operator<(const State* rhs) const {
            return compare(*rhs) < 0;
        }
        
        int State::compare(const State& rhs) const {
            if (m_boundViolation) {
                if (rhs.m_boundViolation)
                    return 0;
                return -1;
            } else if (rhs.m_boundViolation)
                return 1;
            return m_netState.compare(rhs.m_netState);
        }
        
        const Interval::NetState& State::getNetState() const {
            return m_netState;
        }

        bool State::isFinal() const {
            return m_final;
        }

        void State::setFinal(bool final) {
            m_final = final;
        }
        
        bool State::isBoundViolation() const {
            return m_boundViolation;
        }

        const Behavior::State* State::getSuccessor(const String& edgeLabel) const {
            const OutgoingList& edges = getOutgoing();
            OutgoingList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                if (edge->getLabel() == edgeLabel)
                    return edge->getTarget();
            }
            throw AutomatonException("No successor with edge label '" + edgeLabel + "' found");
        }

        String State::asString(const String separator) const {
            if (m_boundViolation)
                return "!";
            return m_netState.asString(separator);
        }

        class StateNetStateComparator {
        public:
            bool operator()(const State* lhs, const State* rhs) const {
                return lhs < rhs;
            }
            bool operator()(const State* lhs, const Interval::NetState& rhs) const {
                return lhs->getNetState() < rhs;
            }
            bool operator()(const Interval::NetState& lhs, const State* rhs) const {
                return lhs < rhs->getNetState();
            }
            bool operator()(const Interval::NetState& lhs, const Interval::NetState& rhs) const {
                return lhs < rhs;
            }
        };

        Automaton::Automaton() :
        m_initialState(NULL),
        m_boundViolationState(NULL) {}

        Automaton::~Automaton() {
            VectorUtils::clearAndDelete(m_states);
            VectorUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        State* Automaton::createState(const Interval::NetState& netState) {
            State* state = new State(netState);
            if (!VectorUtils::setInsert(m_states, state)) {
                delete state;
                throw AutomatonException("Behavior already contains a state with net state '" + netState.asString() + "'");
            }
            return state;
        }
        
        std::pair<State*, bool> Automaton::findOrCreateState(const Interval::NetState& netState) {
            typedef std::pair<State::List::iterator, bool> FindResult;
            FindResult result = VectorUtils::setFind<State*, const Interval::NetState&, StateNetStateComparator>(m_states, netState);
            if (result.second)
                return std::make_pair(*result.first, false);
            
            State* state = new State(netState);
            const bool success = VectorUtils::setInsert(m_states, state, result);
            assert(success);
            return std::make_pair(state, true);
        }

        State* Automaton::findOrCreateBoundViolationState() {
            if (m_boundViolationState == NULL)
                m_boundViolationState = new State();
            return m_boundViolationState;
        }

        Edge* Automaton::connect(State* source, State* target, const String& label) {
            assert(source != NULL);
            assert(source != m_boundViolationState);
            assert(target != NULL);
            assert(VectorUtils::setFind(m_states, source).second);
            assert(target == m_boundViolationState || VectorUtils::setFind(m_states, target).second);
            
            Edge* edge = new Edge(source, target, label);
            const std::pair<Edge::List::iterator, bool> result = VectorUtils::setFind(m_edges, edge);
            if (result.second) {
                delete edge;
                return *result.first;
            } else {
                m_edges.insert(result.first, edge);
                source->addOutgoing(edge);
                target->addIncoming(edge);
                return edge;
            }
        }
        
        void Automaton::deleteState(State* state) {
            assert(state != NULL);
            assert(VectorUtils::setFind(m_states, state).second);
            
            deleteIncomingEdges(state);
            deleteOutgoingEdges(state);
            VectorUtils::setRemoveAndDelete(m_states, state);
        }
        
        void Automaton::disconnect(Edge* edge) {
            assert(edge != NULL);
            assert(VectorUtils::setFind(m_edges, edge).second);
            
            edge->removeFromSource();
            edge->removeFromTarget();
            VectorUtils::setRemoveAndDelete(m_edges, edge);
        }
        
        void Automaton::setInitialState(State* state) {
            m_initialState = state;
        }
        
        void Automaton::addFinalState(State* state) {
            if (!state->isFinal())
                throw AutomatonException("State is not a final state: '" + state->asString() + "'");
            VectorUtils::setInsert(m_finalStates, state);
        }
        
        const State::List& Automaton::getStates() const {
            return m_states;
        }

        const State* Automaton::findState(const Interval::NetState& netState) const {
            typedef std::pair<State::List::const_iterator, bool> FindResult;
            const FindResult result = VectorUtils::setFind<State*, const Interval::NetState&, StateNetStateComparator>(m_states, netState);
            if (!result.second)
                return NULL;
            return *result.first;
        }
        
        State* Automaton::getInitialState() const {
            return m_initialState;
        }
        
        const State::List& Automaton::getFinalStates() const {
            return m_finalStates;
        }

        void Automaton::deleteIncomingEdges(State* state) {
            const State::IncomingList& incomingEdges = state->getIncoming();
            State::IncomingList::const_iterator it, end;
            for (it = incomingEdges.begin(), end = incomingEdges.end(); it != end; ++it) {
                Edge* edge = *it;
                edge->removeFromSource();
                VectorUtils::setRemoveAndDelete(m_edges, edge);
            }
        }
        
        void Automaton::deleteOutgoingEdges(State* state) {
            const State::OutgoingList& outgoingEdges = state->getOutgoing();
            State::OutgoingList::const_iterator it, end;
            for (it = outgoingEdges.begin(), end = outgoingEdges.end(); it != end; ++it) {
                Edge* edge = *it;
                edge->removeFromTarget();
                VectorUtils::setRemoveAndDelete(m_edges, edge);
            }
        }
    }
}
