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

#include "ConstructRegionAutomaton.h"

#include "Region.h"

namespace Tippi {
    ConstructRegionAutomaton::RePtr ConstructRegionAutomaton::operator()(const ClPtr closureAutomaton) {
        RePtr regionAutomaton(new ReAutomaton());
        
        const ClState::Set& clStates = closureAutomaton->getStates();
        ClState::resetVisited(clStates.begin(), clStates.end());
        
        ClState* initialCl = closureAutomaton->getInitialState();
        if (initialCl != NULL)
            buildRegion(initialCl, regionAutomaton);
        
        return regionAutomaton;
    }
    
    ReState* ConstructRegionAutomaton::buildRegion(ClState* state, RePtr automaton) const {
        assert(state != NULL);
        assert(!state->isEmpty());
        if (state->isVisited()) {
            ReState* regionState = automaton->findRegion(state);
            assert(regionState != NULL);
            return regionState;
        }
        
        ClState::Set region;
        growRegion(state, region);
        ReState* regionState = automaton->createState(region);
        buildSuccessors(regionState, automaton);
        return regionState;
    }
    
    void ConstructRegionAutomaton::buildSuccessors(ReState* region, RePtr automaton) const {
        const ClState::Set& clStates = region->getRegion();
        ClState::Set::const_iterator it, end;
        for (it = clStates.begin(), end = clStates.end(); it != end; ++it) {
            ClState* clState = *it;
            buildSuccessors(region, clState, automaton);
        }
    }

    void ConstructRegionAutomaton::buildSuccessors(ReState* region, ClState* state, RePtr automaton) const {
        const ClEdge::List& outgoing = state->getOutgoing();
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            ClEdge* edge = *it;
            if (edge->isPartnerAction()) {
                ClState* target = edge->getTarget();
                if (!target->isEmpty()) {
                    ReState* succRegion = buildRegion(target, automaton);
                    automaton->connect(region, succRegion, edge->getLabel());
                }
            }
        }
    }

    void ConstructRegionAutomaton::growRegion(ClState* state, ClState::Set& region) const {
        if (!state->isVisited() && !state->isEmpty()) {
            region.insert(state);
            state->setVisited(true);
            
            const ClEdge::List& incoming = state->getIncoming();
            const ClEdge::List& outgoing = state->getOutgoing();
            
            growIncoming(incoming, region);
            growOutgoing(outgoing, region);
        }
    }
    
    void ConstructRegionAutomaton::growIncoming(const ClEdge::List& incoming, ClState::Set& region) const {
        ClEdge::List::const_iterator it, end;
        for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
            ClEdge* edge = *it;
            if (edge->isServiceAction() || edge->isTimeAction()) {
                ClState* source = edge->getSource();
                growRegion(source, region);
            }
        }
    }
    
    void ConstructRegionAutomaton::growOutgoing(const ClEdge::List& outgoing, ClState::Set& region) const {
        typedef std::pair<SuccessorMap::iterator, bool> InsertPos;
        
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            ClEdge* edge = *it;
            if (edge->isServiceAction() || edge->isTimeAction()) {
                ClState* target = edge->getTarget();
                growRegion(target, region);
            }
        }
    }
}
