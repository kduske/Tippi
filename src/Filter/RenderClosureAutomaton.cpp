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

#include "RenderClosureAutomaton.h"

#include "CollectionUtils.h"
#include "Closure.h"
#include "Graph/GraphAlgorithms.h"
#include "Region.h"

#include <iomanip>

namespace Tippi {
    class ClosureVisitor {
    private:
        typedef std::map<const ClState*, size_t> ClIdMap;
        typedef std::map<const ReState*, size_t> ReIndexMap;
        
        std::ostream& m_stream;
        size_t m_stateId;
        ClIdMap m_stateIdMap;
        size_t m_regionIndex;
        ReIndexMap m_regionIndexes;
        
        size_t m_maxDeadlockDistance;
        RenderClosureAutomaton::RePtr m_regionAutomaton;
        bool m_showEmptyState;
    public:
        ClosureVisitor(std::ostream& stream, const size_t maxDeadlockDistance, const bool showEmptyState, RenderClosureAutomaton::RePtr regionAutomaton = RenderClosureAutomaton::RePtr()) :
        m_stream(stream),
        m_stateId(1),
        m_regionIndex(0),
        m_maxDeadlockDistance(maxDeadlockDistance),
        m_regionAutomaton(regionAutomaton),
        m_showEmptyState(showEmptyState) {}
        
        void operator()(const ClState* state) {
            if (m_showEmptyState || !state->isEmpty()) {
                m_stream << getStateId(state) << " [";
                printAttribute("label", state->asString("\n", "\n"));
                m_stream << ",";
                printAttribute("shape", "ellipse");
                if (state->isFinal()) {
                    m_stream << ",";
                    printAttribute("peripheries", "2");
                }
                
                if (m_regionAutomaton == NULL) {
                    const size_t distance = state->getDeadlockDistance();
                    if (!state->isReachable()) {
                        m_stream << ",";
                        printAttribute("style", "filled");
                        m_stream << ",";
                        printColorAttribute("fillcolor", 0, 255, 0);
                    } else if (distance > 0 && m_maxDeadlockDistance > 0) {
                        const float d = distance;
                        const float m = m_maxDeadlockDistance;
                        const size_t gb = static_cast<size_t>(d / m * 255.0f);
                        m_stream << ",";
                        printAttribute("style", "filled");
                        m_stream << ",";
                        printColorAttribute("fillcolor", 255, gb, gb);
                    }
                } else if (!state->isEmpty()) {
                    const ReState* region = m_regionAutomaton->findRegion(state);
                    const size_t index = getRegionIndex(region);
                    const size_t count = m_regionAutomaton->getStates().size();
                    const float hue = static_cast<float>(index) / static_cast<float>(count);
                    m_stream << ",";
                    printAttribute("style", "filled");
                    m_stream << ",";
                    m_stream << "fillcolor=\"" << hue << " 0.3 0.9\"";
                }
                m_stream << "];" << std::endl;
            }
        }
        
        void operator()(const ClEdge* edge) {
            const ClState* source = edge->getSource();
            const ClState* target = edge->getTarget();
            
            if (m_showEmptyState ||
                (!source->isEmpty() &&
                 !target->isEmpty())) {
                    m_stream << getStateId(source) << " -> " << getStateId(target) << " [";
                    printAttribute("label", edge->getLabel());
                    
                    if (source->isEmpty() || target->isEmpty()) {
                        m_stream << ",";
                        printAttribute("color", "0.0 0.0 0.7");
                        m_stream << ",";
                        printAttribute("fontcolor", "0.0 0.0 0.7");
                    } else if (edge->isPartnerAction()) {
                        m_stream << ",";
                        printAttribute("color", "0.0 0.0 0.2");
                        m_stream << ",";
                        printAttribute("fontcolor", "0.0 0.0 0.2");
                    } else {
                        m_stream << ",";
                        printAttribute("penwidth", "2");
                    }
                    m_stream << "];" << std::endl;
                }
            
        }
    private:
        size_t getStateId(const ClState* state) {
            std::pair<ClIdMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_stateIdMap, state);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t stateId = m_stateId++;
            m_stateIdMap.insert(insertPos.first, std::make_pair(state, stateId));
            return stateId;
        }
        
        size_t getRegionIndex(const ReState* region) {
            std::pair<ReIndexMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_regionIndexes, region);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t index = m_regionIndex++;
            m_regionIndexes.insert(insertPos.first, std::make_pair(region, index));
            return index;
        }
        
        void printAttribute(const String& name, const String& value) {
            m_stream << name << "=\"" << value << "\"";
        }
        
        void printColorAttribute(const String& name, size_t r, size_t g, size_t b) {
            m_stream << name << "=\"#";
            m_stream << std::hex;
            m_stream << std::setfill('0') << std::setw(2) << r;
            m_stream << std::setfill('0') << std::setw(2) << g;
            m_stream << std::setfill('0') << std::setw(2) << b;
            m_stream << "\"";
            m_stream << std::dec;
        }
    };
    
    RenderClosureAutomaton::RenderClosureAutomaton(const bool showEmptyState) :
    m_showEmptyState(showEmptyState) {}

    void RenderClosureAutomaton::operator()(const ClPtr automaton, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        ClosureVisitor visitor(stream, automaton->getMaxDeadlockDistance(), m_showEmptyState);
        const ClState::Set& states = automaton->getStates();
        ClState::resetVisited(states.begin(), states.end());
        
        ClState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const ClState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
    
    void RenderClosureAutomaton::operator()(const ClPtr closureAutomaton, const RePtr regionAutomaton, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        ClosureVisitor visitor(stream, closureAutomaton->getMaxDeadlockDistance(), m_showEmptyState, regionAutomaton);
        const ClState::Set& states = closureAutomaton->getStates();
        ClState::resetVisited(states.begin(), states.end());
        
        ClState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const ClState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
}
