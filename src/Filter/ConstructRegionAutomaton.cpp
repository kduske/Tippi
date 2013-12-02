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
        buildRegion(initialCl, regionAutomaton);
        
        return regionAutomaton;
    }

    void ConstructRegionAutomaton::buildRegion(ClState* state, RePtr automaton) const {
        assert(state != NULL);
        if (!state->isVisited() && !state->isEmpty()) {
            ClState::Set region;
            growRegion(state, region, automaton);
            automaton->createState(region);
        }
    }

    void ConstructRegionAutomaton::growRegion(ClState* state, ClState::Set& region, RePtr automaton) const {
        if (!state->isVisited() && !state->isEmpty()) {
            assert(region.insert(state).second);
            state->setVisited(true);
            
            const ClEdge::List& incoming = state->getIncoming();
            const ClEdge::List& outgoing = state->getOutgoing();
            
            growIncoming(incoming, region, automaton);
            growOutgoing(outgoing, region, automaton);
        }
    }

    void ConstructRegionAutomaton::growIncoming(const ClEdge::List& incoming, ClState::Set& region, RePtr automaton) const {
        ClEdge::List::const_iterator it, end;
        for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
            ClEdge* edge = *it;
            if (edge->isServiceAction() || edge->isTimeAction()) {
                ClState* source = edge->getSource();
                growRegion(source, region, automaton);
            }
        }
    }
    
    void ConstructRegionAutomaton::growOutgoing(const ClEdge::List& outgoing, ClState::Set& region, RePtr automaton) const {
        ClEdge::List::const_iterator it, end;
        for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
            ClEdge* edge = *it;
            ClState* target = edge->getTarget();
            if (edge->isServiceAction() || edge->isTimeAction()) {
                growRegion(target, region, automaton);
            } else {
                assert(edge->isPartnerAction());
                ClState* target = edge->getTarget();
                buildRegion(target, automaton);
            }
        }
    }
}
