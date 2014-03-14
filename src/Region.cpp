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

#include "Region.h"

#include "Exceptions.h"

namespace Tippi {
    RegionEdge::RegionEdge(RegionState* source, RegionState* target, const String& label) :
    AutomatonEdge(source, target, label) {}
    
    int RegionState::KeyCmp::operator() (const Key& lhs, const Key& rhs) const {
        Region::const_iterator lit = lhs.begin();
        const Region::const_iterator lend = rhs.end();
        Region::const_iterator rit = rhs.begin();
        const Region::const_iterator rend = rhs.end();
        
        while (lit != lend && rit != rend) {
            const ClosureState* lstate = *lit;
            const ClosureState* rstate = *rit;
            const int cmp = m_cmp(ClosureState::getKey(lstate),
                                  ClosureState::getKey(rstate));
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

    RegionState::RegionState(const Region& region) :
    m_region(region) {}
    
    const RegionState::Key& RegionState::getKey(const RegionState* state) {
        return state->getRegion();
    }

    const Region& RegionState::getRegion() const {
        return m_region;
    }

    bool RegionState::isEmpty() const {
        return m_region.empty();
    }
    
    const RegionState* RegionAutomaton::findState(const Region& region) const {
        RegionState query(region);
        StateSet::const_iterator it = getStates().find(&query);
        if (it == getStates().end())
            return NULL;
        return *it;
    }
    
    const RegionState* RegionAutomaton::findRegion(const ClosureState* state) const {
        const RegionMap::const_iterator it = m_regions.find(state);
        if (it == m_regions.end())
            return NULL;
        return it->second;
    }

    RegionState* RegionAutomaton::findRegion(const ClosureState* state) {
        const RegionMap::iterator it = m_regions.find(state);
        if (it == m_regions.end())
            return NULL;
        return it->second;
    }
    void RegionAutomaton::stateWasAdded(RegionState* state) {
        updateRegionMap(state);
    }

    void RegionAutomaton::updateRegionMap(RegionState* state) {
        typedef std::pair<RegionMap::iterator, bool> InsertPos;

        const Region& region = state->getRegion();
        Region::const_iterator it, end;
        for (it = region.begin(), end = region.end(); it != end; ++it) {
            const ClosureState* clState = *it;
            
            const InsertPos insertPos = MapUtils::findInsertPos(m_regions, clState);
            assert(!insertPos.second);
            m_regions.insert(insertPos.first, std::make_pair(clState, state));
        }
    }
}
