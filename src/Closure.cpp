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

#include "Closure.h"

#include "CollectionUtils.h"
#include "Exceptions.h"

#include <algorithm>
#include <cassert>

namespace Tippi {
    ClEdge::ClEdge(ClState* source, ClState* target, const String& label) :
    GraphEdge(source, target),
    m_label(label) {}
    
    bool ClEdge::operator<(const ClEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool ClEdge::operator<(const ClEdge* rhs) const {
        return compare(*rhs) < 0;
    }
    
    int ClEdge::compare(const ClEdge& rhs) const {
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
    
    const String& ClEdge::getLabel() const {
        return m_label;
    }
    
    Closure::Closure() {}
    
    Closure::Closure(NetStateSet& netStates) {
        std::swap(m_netStates, netStates);
    }
    
    bool Closure::operator<(const Closure& rhs) const {
        return compare(rhs) < 0;
    }
    
    int Closure::compare(const Closure& rhs) const {
        NetStateSet::const_iterator lit = m_netStates.begin();
        const NetStateSet::const_iterator lend = m_netStates.end();
        NetStateSet::const_iterator rit = rhs.m_netStates.begin();
        const NetStateSet::const_iterator rend = rhs.m_netStates.end();
        
        while (lit != lend && rit != rend) {
            const Interval::NetState& lstate = *lit;
            const Interval::NetState& rstate = *rit;
            const int cmp = lstate.compare(rstate);
            if (cmp < 0)
                return -1;
            if (cmp > 0)
                return 1;
            ++lit;
            ++rit;
        }
        if (lit != lend)
            return 1;
        if (rit != rend)
            return -1;
        return 0;
    }
    
    ClState::ClState(const Closure& closure) :
    m_closure(closure),
    m_final(false) {
    }
    
    bool ClState::operator<(const ClState& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool ClState::operator<(const ClState* rhs) const {
        return compare(*rhs) < 0;
    }
    
    int ClState::compare(const ClState& rhs) const {
        return m_closure < rhs.m_closure;
    }
    
    const Closure& ClState::getClosure() const {
        return m_closure;
    }
    
    bool ClState::isFinal() const {
        return m_final;
    }
    
    void ClState::setFinal(bool final) {
        m_final = final;
    }
    
    const ClState* ClState::getSuccessor(const String& edgeLabel) const {
        const OutgoingList& edges = getOutgoing();
        OutgoingList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            const ClEdge* edge = *it;
            if (edge->getLabel() == edgeLabel)
                return edge->getTarget();
        }
        throw AutomatonException("No successor with edge label '" + edgeLabel + "' found");
    }
    
    class StateCmp {
    public:
        bool operator()(const ClState* lhs, const ClState* rhs) const {
            return lhs < rhs;
        }
        bool operator()(const ClState* lhs, const Closure& rhs) const {
            return lhs->getClosure() < rhs;
        }
        bool operator()(const Closure& lhs, const ClState* rhs) const {
            return lhs < rhs->getClosure();
        }
        bool operator()(const Closure& lhs, const Closure& rhs) const {
            return lhs < rhs;
        }
    };
    
    ClAutomaton::ClAutomaton() :
    m_initialState(NULL) {}
    
    ClAutomaton::~ClAutomaton() {
        VectorUtils::clearAndDelete(m_states);
        VectorUtils::clearAndDelete(m_edges);
        m_initialState = NULL;
        m_finalStates.clear();
    }
    
    ClState* ClAutomaton::createState(const Closure& closure) {
        ClState* state = new ClState(closure);
        if (!VectorUtils::setInsert(m_states, state)) {
            delete state;
            throw AutomatonException("Behavior already contains a state with the given closure");
        }
        return state;
    }
    
    std::pair<ClState*, bool> ClAutomaton::findOrCreateState(const Closure& closure) {
        typedef std::pair<ClState::List::iterator, bool> FindResult;
        FindResult result = VectorUtils::setFind<ClState*, const Closure&, StateCmp>(m_states, closure);
        if (result.second)
            return std::make_pair(*result.first, false);
        
        ClState* state = new ClState(closure);
        const bool success = VectorUtils::setInsert(m_states, state, result);
        assert(success);
        return std::make_pair(state, true);
    }
    
    ClEdge* ClAutomaton::connect(ClState* source, ClState* target, const String& label) {
        assert(source != NULL);
        assert(target != NULL);
        assert(VectorUtils::setFind(m_states, source).second);
        assert(VectorUtils::setFind(m_states, target).second);
        
        ClEdge* edge = new ClEdge(source, target, label);
        if (!VectorUtils::setInsert(m_edges, edge)) {
            delete edge;
            throw AutomatonException("Behavior already contains an edge from connecting the given states");
        }
        source->addOutgoing(edge);
        target->addIncoming(edge);
        return edge;
    }
    
    void ClAutomaton::deleteState(ClState* state) {
        assert(state != NULL);
        assert(VectorUtils::setFind(m_states, state).second);
        
        deleteIncomingEdges(state);
        deleteOutgoingEdges(state);
        VectorUtils::setRemoveAndDelete(m_states, state);
    }
    
    void ClAutomaton::disconnect(ClEdge* edge) {
        assert(edge != NULL);
        assert(VectorUtils::setFind(m_edges, edge).second);
        
        edge->removeFromSource();
        edge->removeFromTarget();
        VectorUtils::setRemoveAndDelete(m_edges, edge);
    }
    
    void ClAutomaton::setInitialState(ClState* state) {
        m_initialState = state;
    }
    
    void ClAutomaton::addFinalState(ClState* state) {
        if (!state->isFinal())
            throw AutomatonException("The given state is not a final state");
        VectorUtils::setInsert(m_finalStates, state);
    }
    
    const ClState::List& ClAutomaton::getStates() const {
        return m_states;
    }
    
    const ClState* ClAutomaton::findState(const Closure& closure) const {
        typedef std::pair<ClState::List::const_iterator, bool> FindResult;
        const FindResult result = VectorUtils::setFind<ClState*, const Closure&, StateCmp>(m_states, closure);
        if (!result.second)
            return NULL;
        return *result.first;
    }
    
    ClState* ClAutomaton::getInitialState() const {
        return m_initialState;
    }
    
    const ClState::List& ClAutomaton::getFinalStates() const {
        return m_finalStates;
    }
    
    void ClAutomaton::deleteIncomingEdges(ClState* state) {
        const ClState::IncomingList& incomingEdges = state->getIncoming();
        ClState::IncomingList::const_iterator it, end;
        for (it = incomingEdges.begin(), end = incomingEdges.end(); it != end; ++it) {
            ClEdge* edge = *it;
            edge->removeFromSource();
            VectorUtils::setRemoveAndDelete(m_edges, edge);
        }
    }
    
    void ClAutomaton::deleteOutgoingEdges(ClState* state) {
        const ClState::OutgoingList& outgoingEdges = state->getOutgoing();
        ClState::OutgoingList::const_iterator it, end;
        for (it = outgoingEdges.begin(), end = outgoingEdges.end(); it != end; ++it) {
            ClEdge* edge = *it;
            edge->removeFromTarget();
            VectorUtils::setRemoveAndDelete(m_edges, edge);
        }
    }
}