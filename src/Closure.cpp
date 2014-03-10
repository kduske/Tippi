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

#include "Closure.h"

#include "CollectionUtils.h"
#include "Exceptions.h"

#include <algorithm>
#include <cassert>

namespace Tippi {
    ClEdge::ClEdge(ClState* source, ClState* target, const String& label, const Type type) :
    GraphEdge(source, target),
    m_label(label),
    m_type(type) {}
    
    bool ClEdge::operator<(const ClEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool ClEdge::operator==(const ClEdge& rhs) const {
        return compare(rhs) == 0;
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
    
    ClEdge::Type ClEdge::getType() const {
        return m_type;
    }
    
    bool ClEdge::isServiceAction() const {
        return m_type == OutputSend || m_type == InputRead;
    }
    
    bool ClEdge::isPartnerAction() const {
        return m_type == InputSend || m_type == OutputRead;
    }
    
    bool ClEdge::isTimeAction() const {
        return m_type == Time;
    }

    Closure::Closure() {}
    
    Closure::Closure(const Interval::NetState::Set& netStates) :
    m_netStates(netStates) {}
    
    bool Closure::operator<(const Closure& rhs) const {
        return compare(rhs) < 0;
    }
    
    int Closure::compare(const Closure& rhs) const {
        Interval::NetState::Set::const_iterator lit = m_netStates.begin();
        const Interval::NetState::Set::const_iterator lend = m_netStates.end();
        Interval::NetState::Set::const_iterator rit = rhs.m_netStates.begin();
        const Interval::NetState::Set::const_iterator rend = rhs.m_netStates.end();
        
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
    
    bool Closure::contains(const Interval::NetState& state) const {
        return m_netStates.count(state) > 0;
    }

    const Interval::NetState::Set& Closure::getStates() const {
        return m_netStates;
    }

    bool Closure::isEmpty() const {
        return m_netStates.empty();
    }

    String Closure::asString(const String& markingSeparator, const String& stateSeparator) const {
        StringStream result;
        Interval::NetState::Set::const_iterator it, end;
        for (it = m_netStates.begin(), end = m_netStates.end(); it != end; ++it) {
            const Interval::NetState& state = *it;
            result << state.asString(markingSeparator);
            if (std::distance(it, end) > 1)
                result << stateSeparator;
        }
        return result.str();
    }

    ClState::ClState(const Closure& closure) :
    m_closure(closure),
    m_final(false),
    m_deadlockDistance(0),
    m_reachable(true) {}
    
    bool ClState::operator<(const ClState& rhs) const {
        return compare(rhs) < 0;
    }
    
    int ClState::compare(const ClState& rhs) const {
        return m_closure.compare(rhs.m_closure);
    }
    
    const Closure& ClState::getClosure() const {
        return m_closure;
    }
    
    bool ClState::isEmpty() const {
        return m_closure.isEmpty();
    }

    bool ClState::isFinal() const {
        return m_final;
    }
    
    void ClState::setFinal(bool final) {
        m_final = final;
    }
    
    bool ClState::isDeadlock() const {
        if (isFinal())
            return false;
        if (m_closure.getStates().empty())
            return false;
        
        const ClEdge::List& outgoing = getOutgoing();
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            const ClEdge* edge = *it;
            const ClState* successor = edge->getTarget();
            if (successor != this && !successor->getClosure().getStates().empty())
                return false;
        }
        
        return true;
    }

    size_t ClState::getDeadlockDistance() const {
        return m_deadlockDistance;
    }
    
    void ClState::setDeadlockDistance(size_t deadlockDistance) {
        m_deadlockDistance = deadlockDistance;
    }

    bool ClState::isReachable() const {
        return m_reachable;
    }
    
    void ClState::setReachable(bool reachable) {
        m_reachable = reachable;
    }

    bool ClState::hasIncomingEdge(const String& edgeLabel) const {
        return !getPredecessors(edgeLabel).empty();
    }
    
    bool ClState::hasOutgoingEdge(const String& edgeLabel) const {
        return getSuccessor(edgeLabel) != NULL;
    }

    ClState::Set ClState::getPredecessors(const String& edgeLabel) const {
        Set result;
        
        const IncomingList& edges = getIncoming();
        IncomingList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            ClEdge* edge = *it;
            if (edge->getLabel() == edgeLabel) {
                ClState* state = edge->getSource();
                result.insert(state);
            }
        }
        return result;
    }

    const ClState* ClState::getSuccessor(const String& edgeLabel) const {
        const OutgoingList& edges = getOutgoing();
        OutgoingList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            const ClEdge* edge = *it;
            if (edge->getLabel() == edgeLabel)
                return edge->getTarget();
        }
        return NULL;
    }
    
    String ClState::asString(const String& markingSeparator, const String& stateSeparator) const {
        return m_closure.asString(markingSeparator, stateSeparator);
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
    m_initialState(NULL),
    m_maxDeadlockDistance(0) {}
    
    ClAutomaton::~ClAutomaton() {
        SetUtils::clearAndDelete(m_states);
        SetUtils::clearAndDelete(m_edges);
        m_initialState = NULL;
        m_finalStates.clear();
    }
    
    ClState* ClAutomaton::createState(const Closure& closure) {
        ClState* state = new ClState(closure);
        ClState::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            throw AutomatonException("Closure automaton already contains a state with the given closure");
        }
        m_states.insert(it, state);
        return state;
    }
    
    std::pair<ClState*, bool> ClAutomaton::findOrCreateState(const Closure& closure) {
        ClState* state = new ClState(closure);
        ClState::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            return std::make_pair(*it, false);
        }
        m_states.insert(it, state);
        return std::make_pair(state, true);
    }
    
    ClEdge* ClAutomaton::connect(ClState* source, ClState* target, const String& label, const ClEdge::Type type) {
        assert(source != NULL);
        assert(target != NULL);
        assert(m_states.count(source) == 1);
        assert(m_states.count(target) == 1);
        
        ClEdge* edge = new ClEdge(source, target, label, type);
        ClEdge::Set::iterator it = m_edges.lower_bound(edge);
        if (it != m_edges.end() && SetUtils::equals(m_edges, edge, *it)) {
            delete edge;
            return *it;
        }
        
        m_edges.insert(it, edge);
        source->addOutgoing(edge);
        target->addIncoming(edge);
        return edge;
    }
    
    void ClAutomaton::deleteState(ClState* state) {
        // std::cout << "   Delete state: " << state->asString(",", ";") << std::endl;

        assert(state != NULL);
        ClState::Set::iterator it = m_states.lower_bound(state);
        assert(it != m_states.end() && SetUtils::equals(m_states, state, *it));
        
        deleteIncomingEdges(state);
        deleteOutgoingEdges(state);
        m_states.erase(it);

        if (m_initialState == state)
            m_initialState = NULL;
        SetUtils::remove(m_finalStates, state);
        delete state;
    }
    
    void ClAutomaton::disconnect(ClEdge* edge) {
        assert(edge != NULL);
        
        ClEdge::Set::iterator it = m_edges.find(edge);
        assert(it != m_edges.end());
        
        edge->removeFromSource();
        edge->removeFromTarget();
        m_edges.erase(it);
        delete edge;
    }
    
    void ClAutomaton::setInitialState(ClState* state) {
        m_initialState = state;
    }
    
    void ClAutomaton::addFinalState(ClState* state) {
        if (!state->isFinal())
            throw AutomatonException("The given state is not a final state");
        m_finalStates.insert(state);
    }
    
    const ClState::Set& ClAutomaton::getStates() const {
        return m_states;
    }
    
    const ClState* ClAutomaton::findState(const Closure& closure) const {
        ClState query(closure);
        ClState::Set::const_iterator it = m_states.find(&query);
        if (it == m_states.end())
            return NULL;
        return *it;
    }
    
    ClState* ClAutomaton::getInitialState() const {
        return m_initialState;
    }
    
    const ClState::Set& ClAutomaton::getFinalStates() const {
        return m_finalStates;
    }
    
    size_t ClAutomaton::getMaxDeadlockDistance() const {
        return m_maxDeadlockDistance;
    }
    
    void ClAutomaton::setMaxDeadlockDistnace(size_t maxDeadlockDistance) {
        m_maxDeadlockDistance = maxDeadlockDistance;
    }

    ClState::Set ClAutomaton::findUnreachableStates() const {
        ClState::Set unreachable;
        
        size_t count;
        do {
            count = unreachable.size();
            doFindUnreachableStates(unreachable);
        } while (count < unreachable.size());
        
        return unreachable;
    }

    void ClAutomaton::doFindUnreachableStates(ClState::Set& unreachable) const {
        ClState::Set::const_iterator it, end;
        for (it = m_states.begin(), end = m_states.end(); it != end; ++it) {
            ClState* state = *it;
            if (state != m_initialState &&
                unreachable.count(state) == 0 &&
                state->isPresetSubsetOfIgnoringLoops(unreachable))
                unreachable.insert(state);
        }
        
    }
    
    void ClAutomaton::deleteIncomingEdges(ClState* state) {
        const ClState::IncomingList& incomingEdges = state->getIncoming();
        ClState::IncomingList::const_iterator it, end;
        for (it = incomingEdges.begin(), end = incomingEdges.end(); it != end; ++it) {
            ClEdge* edge = *it;
            SetUtils::remove(m_edges, edge);
            edge->removeFromSource();
            delete edge;
        }
    }
    
    void ClAutomaton::deleteOutgoingEdges(ClState* state) {
        const ClState::OutgoingList& outgoingEdges = state->getOutgoing();
        ClState::OutgoingList::const_iterator it, end;
        for (it = outgoingEdges.begin(), end = outgoingEdges.end(); it != end; ++it) {
            ClEdge* edge = *it;
            SetUtils::remove(m_edges, edge);
            edge->removeFromTarget();
            delete edge;
        }
    }
}