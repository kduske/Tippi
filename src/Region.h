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

#ifndef __Tippi__Region__
#define __Tippi__Region__

#include "CollectionUtils.h"
#include "SharedPointer.h"
#include "StringUtils.h"
#include "Automaton.h"
#include "Closure.h"

#include <set>
#include <vector>

namespace Tippi {
    class RegionState;
    
    class RegionEdge : public AutomatonEdge<RegionState> {
    public:
        typedef std::vector<RegionEdge*> List;
    public:
        RegionEdge(RegionState* source, RegionState* target, const String& label, bool tau);
    };

    typedef ClosureAutomaton::StateSet Region;
    
    class RegionState : public AutomatonState<RegionEdge> {
    public:
        typedef Region Key;
        struct KeyCmp {
            ClosureState::KeyCmp m_cmp;
            int operator() (const Key& lhs, const Key& rhs) const;
        };
    private:
        Region m_region;
    public:
        RegionState(const Region& region);
        static const Key getKey(const RegionState* state);

        const Region& getRegion() const;
        bool isEmpty() const;
    };
    
    class RegionAutomaton : public Automaton<RegionState, RegionEdge> {
    public:
        typedef std::tr1::shared_ptr<RegionAutomaton> Ptr;
    private:
        typedef std::map<const ClosureState*, RegionState*> RegionMap;
        RegionMap m_regions;
    public:
        const RegionState* findState(const Region& region) const;
        const RegionState* findRegion(const ClosureState* state) const;
        RegionState* findRegion(const ClosureState* state);
    private:
        void stateWasAdded(RegionState* state);
        void updateRegionMap(RegionState* state);
    };
}

#endif /* defined(__Tippi__Region__) */
