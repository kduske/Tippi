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

#ifndef __Tippi__Behavior__
#define __Tippi__Behavior__

#include "StringUtils.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"
#include "Net/IntervalNetState.h"

#include <set>
#include <vector>

namespace Tippi {
    class ClState;
    
    class ClEdge : public GraphEdge<ClState, ClState> {
    public:
        typedef std::vector<ClEdge*> List;
    private:
        String m_label;
    public:
        ClEdge(ClState* source, ClState* target, const String& label);
        
        bool operator<(const ClEdge& rhs) const;
        bool operator<(const ClEdge* rhs) const;
        int compare(const ClEdge& rhs) const;
        
        const String& getLabel() const;
    };
    
    class Closure {
    public:
        typedef std::set<Interval::NetState> NetStateSet;
    private:
        NetStateSet m_netStates;
    public:
        Closure();
        Closure(NetStateSet& netStates);
        
        bool operator<(const Closure& rhs) const;
        int compare(const Closure& rhs) const;
    };
    
    class ClState : public GraphNode<ClEdge, ClEdge> {
    public:
        typedef std::vector<ClState*> List;
    private:
        Closure m_closure;
        bool m_final;
    public:
        ClState(const Closure& closure);
        
        bool operator<(const ClState& rhs) const;
        bool operator<(const ClState* rhs) const;
        int compare(const ClState& rhs) const;
        
        const Closure& getClosure() const;
        bool isFinal() const;
        void setFinal(bool final);
        
        const ClState* getSuccessor(const String& edgeLabel) const;
    };
    
    class ClAutomaton {
    private:
        ClState::List m_states;
        ClEdge::List m_edges;
        ClState* m_initialState;
        ClState::List m_finalStates;
    public:
        ClAutomaton();
        ~ClAutomaton();
        
        ClState* createState(const Closure& closure);
        std::pair<ClState*, bool> findOrCreateState(const Closure& closure);
        ClEdge* connect(ClState* source, ClState* target, const String& label);
        
        void deleteState(ClState* state);
        void disconnect(ClEdge* edge);
        
        void setInitialState(ClState* state);
        void addFinalState(ClState* state);
        
        const ClState::List& getStates() const;
        const ClState* findState(const Closure& closure) const;
        
        ClState* getInitialState() const;
        const ClState::List& getFinalStates() const;
    private:
        void deleteIncomingEdges(ClState* state);
        void deleteOutgoingEdges(ClState* state);
    };
}

#endif /* defined(__Tippi__Behavior__) */