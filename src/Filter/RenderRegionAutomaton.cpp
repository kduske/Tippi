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

#include "RenderRegionAutomaton.h"

#include "CollectionUtils.h"
#include "Closure.h"
#include "Region.h"
#include "Graph/GraphAlgorithms.h"

namespace Tippi {
    class Visitor {
    private:
        typedef std::map<const ReState*, size_t> ReIdMap;
        typedef std::map<const ClState*, size_t> ClIdMap;
        
        std::ostream& m_stream;
        size_t m_stateId;
        ReIdMap m_reIdMap;
        ClIdMap m_clIdMap;
    public:
        Visitor(std::ostream& stream) :
        m_stream(stream),
        m_stateId(1) {}
        
        void operator()(const ReState* state) {
            const size_t stateId = getReStateId(state);
            m_stream << "subgraph cluster_" << stateId << " {" << std::endl;
            
            const ClState::Set& closureStates = state->getRegion();
            renderClosureStates(closureStates);
            
            const ClEdge::Set closureEdges = getClosureEdges(closureStates);
            renderClosureEdges(closureEdges);
            
            m_stream << "}" << std::endl;
            
            /*
             m_stream << stateId << " [";
             m_stream << "label=\"" << stateId << "\"";
             m_stream << ",";
             printAttribute("shape", "ellipse");
             if (state->isFinal()) {
             m_stream << ",";
             printAttribute("peripheries", "2");
             }
             m_stream << "];" << std::endl;
             */
        }
        
        ClEdge::Set getClosureEdges(const ClState::Set& states) {
            ClEdge::Set edges;
            ClState::Set::const_iterator it, end;
            for (it = states.begin(), end = states.end(); it != end; ++it) {
                const ClState* state = *it;
                getClosureEdges(state, edges);
            }
            return edges;
        }
        
        void getClosureEdges(const ClState* state, ClEdge::Set& edges) {
            ClEdge::List::const_iterator it, end;
            
            const ClEdge::List& incoming = state->getIncoming();
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                ClEdge* edge = *it;
                if (edge->isServiceAction() || edge->isTimeAction())
                    edges.insert(edge);
            }
            
            const ClEdge::List& outgoing = state->getOutgoing();
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                ClEdge* edge = *it;
                if ((edge->isServiceAction() || edge->isTimeAction()) &&
                    !edge->getTarget()->isEmpty())
                    edges.insert(edge);
            }
        }
        
        void renderClosureStates(const ClState::Set& states) {
            ClState::Set::const_iterator it, end;
            for (it = states.begin(), end = states.end(); it != end; ++it) {
                const ClState* state = *it;
                renderClosureState(state);
            }
        }
        
        void renderClosureState(const ClState* state) {
            assert(!state->isEmpty());
            
            m_stream << getClStateId(state) << " [";
            printAttribute("label", state->asString("\n", "\n"));
            m_stream << ",";
            printAttribute("shape", "ellipse");
            if (state->isFinal()) {
                m_stream << ",";
                printAttribute("peripheries", "2");
            }
        }
        
        void renderClosureEdges(const ClEdge::Set& edges) {
            ClEdge::Set::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const ClEdge* edge = *it;
                renderClosureEdge(edge);
            }
        }
        
        void renderClosureEdge(const ClEdge* edge) {
            const ClState* source = edge->getSource();
            const ClState* target = edge->getTarget();
            
            assert(!source->isEmpty());
            assert(!target->isEmpty());
            
            if (!source->getClosure().getStates().empty() &&
                !target->getClosure().getStates().empty()) {
                m_stream << getClStateId(source) << " -> " << getClStateId(target) << " [";
                printAttribute("label", edge->getLabel());
                m_stream << "];" << std::endl;
            }
        }
        
        void operator()(const ReEdge* edge) {
            const ReState* source = edge->getSource();
            const ReState* target = edge->getTarget();
            
            m_stream << getReStateId(source) << " -> " << getReStateId(target) << " [";
            printAttribute("label", edge->getLabel());
            m_stream << "];" << std::endl;
            
        }
    private:
        size_t getReStateId(const ReState* state) {
            std::pair<ReIdMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_reIdMap, state);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t stateId = m_stateId++;
            m_reIdMap.insert(insertPos.first, std::make_pair(state, stateId));
            return stateId;
        }
        
        size_t getClStateId(const ClState* state) {
            std::pair<ClIdMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_clIdMap, state);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t stateId = m_stateId++;
            m_clIdMap.insert(insertPos.first, std::make_pair(state, stateId));
            return stateId;
        }
        
        void printAttribute(const String& name, const String& value) {
            m_stream << name << "=\"" << value << "\"";
        }
    };
    
    void RenderRegionAutomaton::operator()(const RePtr automaton, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        stream << "compound=true" << std::endl;
        
        Visitor visitor(stream);
        const ReState::Set& states = automaton->getStates();
        ReState::resetVisited(states.begin(), states.end());
        
        ReState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const ReState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
}
