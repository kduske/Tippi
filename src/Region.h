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
#include "StringUtils.h"
#include "Closure.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"

#include <set>
#include <vector>

namespace Tippi {
    class ReState;
    
    class ReEdge : public GraphEdge<ReState, ReState> {
    public:
        typedef std::vector<ReEdge*> List;
        typedef std::set<ReEdge*, Utils::UniCmp<ReEdge> > Set;
    private:
        String m_label;
    public:
        ReEdge(ReState* source, ReState* target, const String& label);
        
        bool operator<(const ReEdge& rhs) const;
        bool operator==(const ReEdge& rhs) const;
        int compare(const ReEdge& rhs) const;
        
        const String& getLabel() const;
    };
    
    class ReState : public GraphNode<ReEdge, ReEdge> {
    public:
        typedef std::set<ReState*, Utils::UniCmp<ReState> > Set;
    private:
        ClState::Set m_region;
        bool m_final;
    public:
        ReState(const ClState::Set& region);
        
        bool operator<(const ReState& rhs) const;
        bool operator==(const ReState& rhs) const;
        int compare(const ReState& rhs) const;
        
        const ClState::Set& getRegion() const;
        bool isFinal() const;
        void setFinal(bool final);
        bool isEmpty() const;
    };
    
    class ReAutomaton {
    private:
        typedef std::map<const ClState*, ReState*> RegionMap;
        
        ReState::Set m_states;
        ReEdge::Set m_edges;
        ReState* m_initialState;
        ReState::Set m_finalStates;
        RegionMap m_regions;
    public:
        ReAutomaton();
        ~ReAutomaton();
        
        ReState* createState(const ClState::Set& region);
        std::pair<ReState*, bool> findOrCreateState(const ClState::Set& region);
        ReEdge* connect(ReState* source, ReState* target, const String& label);
        
        void setInitialState(ReState* state);
        void addFinalState(ReState* state);
        
        const ReState::Set& getStates() const;
        const ReState* findState(const ClState::Set& region) const;
        const ReState* findRegion(const ClState* state) const;
        ReState* findRegion(const ClState* state);
        
        ReState* getInitialState() const;
        const ReState::Set& getFinalStates() const;
    private:
        void updateRegionMap(ReState* state);
    };
}

#endif /* defined(__Tippi__Region__) */
