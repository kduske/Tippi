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
#include "Region.h"

#include <iostream>
#include <map>


namespace Tippi {
    class ReAutomaton;
    class RegionState;
    
    struct ConstructRegionAutomaton {
    private:
        typedef std::map<RegionState*, String> SuccessorMap;
        typedef std::map<ClosureState*, RegionState*> StateRegionMap;
        
        StateRegionMap m_regions;
    public:
        RegionAutomaton::Ptr operator()(const ClosureAutomaton::Ptr closureAutomaton);
    private:
        RegionState* buildRegion(ClosureState* state, RegionAutomaton::Ptr automaton) const;
        void buildSuccessors(RegionState* region, RegionAutomaton::Ptr automaton) const;
        void buildSuccessors(RegionState* region, ClosureState* state, RegionAutomaton::Ptr automaton) const;
        void growRegion(ClosureState* state, ClosureAutomaton::StateSet& region) const;
        void growIncoming(const ClosureEdge::List& incoming, ClosureAutomaton::StateSet& region) const;
        void growOutgoing(const ClosureEdge::List& outgoing, ClosureAutomaton::StateSet& region) const;
    };
}

#endif /* defined(__Tippi__ConstructRegionAutomaton__) */
