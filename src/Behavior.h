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

#ifndef __Tippi__Behavior__
#define __Tippi__Behavior__

#include "Automaton.h"
#include "StringUtils.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"
#include "Net/IntervalNetState.h"

#include <set>
#include <vector>

namespace Tippi {
    namespace Behavior {
        class State;
        
        class Edge : public AutomatonEdge<State> {
        public:
            typedef std::vector<Edge*> List;
            typedef std::set<Edge*, Utils::UniCmp<Edge> > Set;
        public:
            Edge(State* source, State* target, const String& label, bool tauEdge);
            
            bool operator<(const Edge& rhs) const;
            bool operator<(const Edge* rhs) const;
            int compare(const Edge& rhs) const;
        };
        
        class State : public AutomatonState<Edge> {
        public:
            typedef std::set<State*, Utils::UniCmp<State> > Set;
        private:
            Interval::NetState m_netState;
            bool m_final;
            bool m_boundViolation;
        public:
            State(const Interval::NetState& netState);
            State();
            
            bool operator<(const State& rhs) const;
            bool operator<(const State* rhs) const;
            int compare(const State& rhs) const;
            
            const Interval::NetState& getNetState() const;
            bool isFinal() const;
            void setFinal(bool final);
            bool isBoundViolation() const;
            
            const Behavior::State* getSuccessor(const String& edgeLabel) const;
            String asString(const String separator = " ") const;
        };
        
        class Automaton : public Tippi::Automaton<State, Edge> {
        private:
            State* m_boundViolationState;
        public:
            Automaton();
            ~Automaton();
            
            State* createState(const Interval::NetState& netState);
            std::pair<State*, bool> findOrCreateState(const Interval::NetState& netState);
            State* findOrCreateBoundViolationState();
        };
    }
}

#endif /* defined(__Tippi__Behavior__) */
