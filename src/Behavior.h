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

#include <vector>

namespace Tippi {
    namespace Behavior {
        class State;
        
        class Edge : public GraphEdge<State, State> {
        public:
            typedef std::vector<Edge*> List;
        private:
            String m_label;
        public:
            Edge(State* source, State* target, const String& label);
            
            bool operator<(const Edge& rhs) const;
            bool operator<(const Edge* rhs) const;
            int compare(const Edge& rhs) const;
            
            const String& getLabel() const;
        };
        
        class State : public GraphNode<Edge, Edge> {
        public:
            typedef std::vector<State*> List;
        private:
            Interval::NetState m_netState;
            bool m_final;
        public:
            State(const Interval::NetState& netState);
            
            bool operator<(const State& rhs) const;
            bool operator<(const State* rhs) const;
            int compare(const State& rhs) const;
            
            const Interval::NetState& getNetState() const;
            bool isFinal() const;
            void setFinal(bool final);
        };
        
        class Automaton {
        private:
            State::List m_states;
            Edge::List m_edges;
            State* m_initialState;
            State::List m_finalStates;
        public:
            Automaton();
            ~Automaton();
            
            State* createState(const Interval::NetState& netState);
            std::pair<State*, bool> findOrCreateState(const Interval::NetState& netState);
            Edge* connect(State* source, State* target, const String& label);
            
            void deleteState(State* state);
            void disconnect(Edge* edge);
            
            void setInitialState(State* state);
            void addFinalState(State* state);
            
            const State::List& getStates() const;
            const State* findState(const Interval::NetState& netState) const;
            
            State* getInitialState() const;
            const State::List& getFinalStates() const;
        private:
            void deleteIncomingEdges(State* state);
            void deleteOutgoingEdges(State* state);
        };
    }
}

#endif /* defined(__Tippi__Behavior__) */
