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

#ifndef __Tippi__Automaton__
#define __Tippi__Automaton__

#include "StringUtils.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"

#include <vector>

namespace Tippi {
    template <typename StateInfo> class State;
    
    template <typename StateInfo>
    class Edge : public GraphEdge<State<StateInfo>, State<StateInfo> > {
    public:
        typedef std::vector<Edge*> List;
    private:
        String m_label;
    public:
        Edge(State<StateInfo>* source, State<StateInfo>* target, const String& label) :
        GraphEdge(source, target),
        m_label(label) {}
        
        bool operator<(const Edge& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool operator<(const Edge* rhs) const {
            return compare(*rhs);
        }
        
        int compare(const Edge& rhs) const {
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
        
        const String& getLabel() const {
            return m_label;
        }
    };
    
    template <typename StateInfo>
    class State : public GraphNode<Edge, Edge> {
    public:
        typedef std::vector<State*> List;
    private:
        StateInfo m_info;
        bool m_final;
    public:
        State(const StateInfo& info);
        
        bool operator<(const State& rhs) const;
        bool operator<(const State* rhs) const;
        int compare(const State& rhs) const;
        
        const StateInfo& getInfo() const;
        bool isFinal() const;
        void setFinal(bool final);
        
        const State* getSuccessor(const String& edgeLabel) const;
    };

    template <typename StateInfo>
    class Automaton {
    private:
        typename State::List m_states;
        typename Edge::List m_edges;
        State* m_initialState;
        typename State::List m_finalStates;
    public:
        Automaton();
        virtual ~Automaton();
        
        State* createState(const StateInfo& info);
        std::pair<State*, bool> findOrCreateState(const StateInfo& info);
        Edge* connect(State* source, State* target, const String& label);
        
        void deleteState(State* state);
        void disconnect(Edge* edge);
        
        void setInitialState(State* state);
        void addFinalState(State* state);
        
        const typename State::List& getStates() const;
        const State* findState(const StateInfo& info) const;
        
        State* getInitialState() const;
        const typename State::List& getFinalStates() const;
    private:
        void deleteIncomingEdges(State* state);
        void deleteOutgoingEdges(State* state);
    };
}

#endif /* defined(__Tippi__Automaton__) */
