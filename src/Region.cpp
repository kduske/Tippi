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

#include "Region.h"

#include "Exceptions.h"

namespace Tippi {
    ReEdge::ReEdge(ReState* source, ReState* target, const String& label) :
    GraphEdge(source, target),
    m_label(label) {}
    
    bool ReEdge::operator<(const ReEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool ReEdge::operator==(const ReEdge& rhs) const {
        return compare(rhs) == 0;
    }
    
    int ReEdge::compare(const ReEdge& rhs) const {
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
    
    const String& ReEdge::getLabel() const {
        return m_label;
    }

    ReState::ReState(const ClState::Set& region) :
    m_region(region),
    m_final(false) {}
    
    bool ReState::operator<(const ReState& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool ReState::operator==(const ReState& rhs) const {
        return compare(rhs) == 0;
    }
    
    int ReState::compare(const ReState& rhs) const {
        ClState::Set::const_iterator lit = m_region.begin();
        const ClState::Set::const_iterator lend = m_region.end();
        ClState::Set::const_iterator rit = rhs.m_region.begin();
        const ClState::Set::const_iterator rend = rhs.m_region.end();
        
        while (lit != lend && rit != rend) {
            const ClState* lstate = *lit;
            const ClState* rstate = *rit;
            const int cmp = lstate->compare(*rstate);
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
    
    const ClState::Set& ReState::getRegion() const {
        return m_region;
    }
    
    bool ReState::isFinal() const {
        return m_final;
    }
    
    void ReState::setFinal(bool final) {
        m_final = final;
    }

    ReAutomaton::ReAutomaton() :
    m_initialState(NULL) {}
    
    ReAutomaton::~ReAutomaton() {
        SetUtils::clearAndDelete(m_states);
        SetUtils::clearAndDelete(m_edges);
        m_initialState = NULL;
        m_finalStates.clear();
    }
    
    ReState* ReAutomaton::createState(const ClState::Set& region) {
        ReState* state = new ReState(region);
        ReState::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            throw AutomatonException("Region automaton already contains a state with the given closure");
        }
        m_states.insert(it, state);
        return state;
    }
    
    std::pair<ReState*, bool> ReAutomaton::findOrCreateState(const ClState::Set& region) {
        ReState* state = new ReState(region);
        ReState::Set::iterator it = m_states.lower_bound(state);
        if (it != m_states.end() && SetUtils::equals(m_states, state, *it)) {
            delete state;
            return std::make_pair(*it, false);
        }
        m_states.insert(it, state);
        return std::make_pair(state, true);
    }
    
    ReEdge* ReAutomaton::connect(ReState* source, ReState* target, const String& label) {
        assert(source != NULL);
        assert(target != NULL);
        assert(m_states.count(source) == 1);
        assert(m_states.count(target) == 1);
        
        ReEdge* edge = new ReEdge(source, target, label);
        ReEdge::Set::iterator it = m_edges.lower_bound(edge);
        if (it != m_edges.end() && SetUtils::equals(m_edges, edge, *it)) {
            delete edge;
            return *it;
        }
        
        m_edges.insert(it, edge);
        source->addOutgoing(edge);
        target->addIncoming(edge);
        return edge;
    }
    
    void ReAutomaton::setInitialState(ReState* state) {
        m_initialState = state;
    }
    
    void ReAutomaton::addFinalState(ReState* state) {
        m_finalStates.insert(state);
    }
    
    const ReState::Set& ReAutomaton::getStates() const {
        return m_states;
    }
    
    const ReState* ReAutomaton::findState(const ClState::Set& region) const {
        ReState query(region);
        ReState::Set::const_iterator it = m_states.find(&query);
        if (it == m_states.end())
            return NULL;
        return *it;
    }
    
    ReState* ReAutomaton::getInitialState() const {
        return m_initialState;
    }
    
    const ReState::Set& ReAutomaton::getFinalStates() const {
        return m_finalStates;
    }
}
