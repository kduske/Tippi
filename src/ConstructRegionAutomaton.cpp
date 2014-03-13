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

#include "ConstructRegionAutomaton.h"

namespace Tippi {
    RegionAutomaton::Ptr ConstructRegionAutomaton::operator()(const ClosureAutomaton::Ptr closureAutomaton) {
        RegionAutomaton::Ptr regionAutomaton(new RegionAutomaton());
        
        const ClosureAutomaton::StateSet& clStates = closureAutomaton->getStates();
        ClosureState::resetVisited(clStates.begin(), clStates.end());
        
        ClosureState* initialCl = closureAutomaton->getInitialState();
        if (initialCl != NULL)
            buildRegion(initialCl, regionAutomaton);
        
        return regionAutomaton;
    }
    
    RegionState* ConstructRegionAutomaton::buildRegion(ClosureState* state, RegionAutomaton::Ptr automaton) const {
        assert(state != NULL);
        assert(!state->isEmpty());
        if (state->isVisited()) {
            RegionState* regionState = automaton->findRegion(state);
            assert(regionState != NULL);
            return regionState;
        }
        
        ClosureAutomaton::StateSet region;
        growRegion(state, region);
        RegionState* regionState = automaton->createState(region);
        buildSuccessors(regionState, automaton);
        return regionState;
    }
    
    void ConstructRegionAutomaton::buildSuccessors(RegionState* region, RegionAutomaton::Ptr automaton) const {
        const ClosureAutomaton::StateSet& clStates = region->getRegion();
        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = clStates.begin(), end = clStates.end(); it != end; ++it) {
            ClosureState* clState = *it;
            buildSuccessors(region, clState, automaton);
        }
    }

    void ConstructRegionAutomaton::buildSuccessors(RegionState* region, ClosureState* state, RegionAutomaton::Ptr automaton) const {
        const ClosureEdge::List& outgoing = state->getOutgoing();
        ClosureEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            ClosureEdge* edge = *it;
            if (edge->isPartnerAction()) {
                ClosureState* target = edge->getTarget();
                if (!target->isEmpty()) {
                    RegionState* succRegion = buildRegion(target, automaton);
                    automaton->connectWithLabeledEdge(region, succRegion, edge->getLabel());
                }
            }
        }
    }

    void ConstructRegionAutomaton::growRegion(ClosureState* state, ClosureAutomaton::StateSet& region) const {
        if (!state->isVisited() && !state->isEmpty()) {
            region.insert(state);
            state->setVisited(true);
            
            const ClosureEdge::List& incoming = state->getIncoming();
            const ClosureEdge::List& outgoing = state->getOutgoing();
            
            growIncoming(incoming, region);
            growOutgoing(outgoing, region);
        }
    }
    
    void ConstructRegionAutomaton::growIncoming(const ClosureEdge::List& incoming, ClosureAutomaton::StateSet& region) const {
        ClosureEdge::List::const_iterator it, end;
        for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
            ClosureEdge* edge = *it;
            if (edge->isServiceAction() || edge->isTimeAction()) {
                ClosureState* source = edge->getSource();
                growRegion(source, region);
            }
        }
    }
    
    void ConstructRegionAutomaton::growOutgoing(const ClosureEdge::List& outgoing, ClosureAutomaton::StateSet& region) const {
        typedef std::pair<SuccessorMap::iterator, bool> InsertPos;
        
        ClosureEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            ClosureEdge* edge = *it;
            if (edge->isServiceAction() || edge->isTimeAction()) {
                ClosureState* target = edge->getTarget();
                growRegion(target, region);
            }
        }
    }
}
