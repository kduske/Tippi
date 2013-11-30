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
#include "Exceptions.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"

#include <cassert>
#include <vector>

namespace Tippi {
    template <typename StateInfoType>
    class State;
    
    template <typename StateInfoType>
    class Edge : public GraphEdge<State<StateInfoType>, State<StateInfoType> > {
    public:
        typedef StateInfoType StateInfo;
        typedef std::vector<Edge*> List;
    private:
        String m_label;
    public:
        Edge(State<StateInfoType>* source, State<StateInfoType>* target, const String& label) :
        GraphEdge<State<StateInfoType>, State<StateInfoType> >(source, target),
        m_label(label) {}
        
        bool operator<(const Edge& rhs) const {
            return compare(rhs) < 0;
        }
        
        bool operator<(const Edge* rhs) const {
            return compare(*rhs);
        }
        
        int compare(const Edge& rhs) const {
            const int sourceResult = this->getSource()->compare(*rhs.getSource());
            if (sourceResult < 0)
                return -1;
            if (sourceResult > 0)
                return 1;
            const int targetResult = this->getTarget()->compare(*rhs.getTarget());
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
    
    template <typename StateInfoType>
    class State : public GraphNode<Edge<StateInfoType>, Edge<StateInfoType> > {
    public:
        typedef StateInfoType StateInfo;
        typedef std::vector<State*> List;
    private:
        StateInfoType m_info;
        bool m_final;
    public:
        State(const StateInfoType& info);
        
        bool operator<(const State& rhs) const;
        bool operator<(const State* rhs) const;
        int compare(const State& rhs) const;
        
        const StateInfoType& getInfo() const;
        bool isFinal() const;
        void setFinal(bool final);
        
        const State* getSuccessor(const String& edgeLabel) const;
    };

    template <class State, class Edge>
    class Automaton {
    private:
        typename State::List m_states;
        typename Edge::List m_edges;
        State* m_initialState;
        typename State::List m_finalStates;

        class InfoCmp {
        public:
            bool operator()(const State* lhs, const State* rhs) const {
                return lhs < rhs;
            }
            bool operator()(const State* lhs, const typename State::Info& rhs) const {
                return lhs->getInfo() < rhs;
            }
            bool operator()(const typename State::Info& lhs, const State* rhs) const {
                return lhs < rhs->getInfo();
            }
            bool operator()(const typename State::Info& lhs, const typename State::Info& rhs) const {
                return lhs < rhs;
            }
        };
    public:
        Automaton() :
        m_initialState(NULL) {}
        
        virtual ~Automaton() {
            VectorUtils::clearAndDelete(m_states);
            VectorUtils::clearAndDelete(m_edges);
            m_initialState = NULL;
            m_finalStates.clear();
        }
        
        State* createState(const typename State::StateInfo& info) {
            State* state = doCreateState(info);
            if (!VectorUtils::setInsert(m_states, state)) {
                delete state;
                throw AutomatonException("Behavior already contains a state with the given info");
            }
            return state;
        }
        
        std::pair<State*, bool> findOrCreateState(const typename State::StateInfo& info) {
            typedef std::pair<typename State::List::iterator, bool> FindResult;
            FindResult result = VectorUtils::setFind<State*, const typename State::StateInfo&, InfoCmp>(m_states, info);
            if (result.second)
                return std::make_pair(*result.first, false);
            
            State* state = doCreateState(info);
            const bool success = VectorUtils::setInsert(m_states, state, result);
            assert(success);
            return std::make_pair(state, true);
        }
        
        Edge* connect(State* source, State* target, const String& label) {
        }
        
        void deleteState(State* state);
        void disconnect(Edge* edge);
        
        void setInitialState(State* state);
        void addFinalState(State* state);
        
        const typename State::List& getStates() const;
        const State* findState(const typename State::StateInfo& info) const;
        
        State* getInitialState() const;
        const typename State::List& getFinalStates() const;
    private:
        void deleteIncomingEdges(State* state);
        void deleteOutgoingEdges(State* state);
        
        virtual State* doCreateState(const typename State::StateInfo& info) = 0;
    };
}

#endif /* defined(__Tippi__Automaton__) */
