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

#ifndef __Tippi__ConstructRegionAutomaton__
#define __Tippi__ConstructRegionAutomaton__

#include "SharedPointer.h"
#include "Closure.h"

#include <iostream>
#include <map>


namespace Tippi {
    class ReAutomaton;
    class ReState;
    
    struct ConstructRegionAutomaton {
    private:
        typedef std::map<ReState*, String> SuccessorMap;
        typedef std::map<ClState*, ReState*> StateRegionMap;
        
        StateRegionMap m_regions;
    public:
        typedef std::tr1::shared_ptr<ClAutomaton> ClPtr;
        typedef std::tr1::shared_ptr<ReAutomaton> RePtr;
        
        RePtr operator()(const ClPtr closureAutomaton);
    private:
        ReState* buildRegion(ClState* state, RePtr automaton) const;
        void buildSuccessors(ReState* region, RePtr automaton) const;
        void buildSuccessors(ReState* region, ClState* state, RePtr automaton) const;
        void growRegion(ClState* state, ClState::Set& region) const;
        void growIncoming(const ClEdge::List& incoming, ClState::Set& region) const;
        void growOutgoing(const ClEdge::List& outgoing, ClState::Set& region) const;
    };
}

#endif /* defined(__Tippi__ConstructRegionAutomaton__) */
