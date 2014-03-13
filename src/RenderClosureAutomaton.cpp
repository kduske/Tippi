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

#include "RenderClosureAutomaton.h"

#include "CollectionUtils.h"
#include "GraphAlgorithms.h"

#include <iomanip>

namespace Tippi {
    class ClosureVisitor {
    private:
        std::ostream& m_stream;
        
        size_t m_maxDeadlockDistance;
        RegionAutomaton::Ptr m_regionAutomaton;
        bool m_showEmptyState;
    public:
        ClosureVisitor(std::ostream& stream, const size_t maxDeadlockDistance, const bool showEmptyState, RegionAutomaton::Ptr regionAutomaton = RegionAutomaton::Ptr()) :
        m_stream(stream),
        m_maxDeadlockDistance(maxDeadlockDistance),
        m_regionAutomaton(regionAutomaton),
        m_showEmptyState(showEmptyState) {}
        
        void operator()(const ClosureState* state) {
            if (m_showEmptyState || !state->isEmpty()) {
                m_stream << "" << state->getId() << " [";
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
                    const RegionState* region = m_regionAutomaton->findRegion(state);
                    const size_t stateId = region->getId();
                    const size_t maxId = m_regionAutomaton->getMaxId();
                    const float hue = static_cast<float>(stateId) / static_cast<float>(maxId);
                    m_stream << ",";
                    printAttribute("style", "filled");
                    m_stream << ",";
                    m_stream << "fillcolor=\"" << hue << " 0.3 0.9\"";
                }
                m_stream << "];" << std::endl;
            }
        }
        
        void operator()(const ClosureEdge* edge) {
            const ClosureState* source = edge->getSource();
            const ClosureState* target = edge->getTarget();
            
            if (m_showEmptyState ||
                (!source->isEmpty() &&
                 !target->isEmpty())) {
                    m_stream << source->getId() << " -> " << target->getId() << " [";
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

    void RenderClosureAutomaton::operator()(const ClosureAutomaton::Ptr automaton, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        ClosureVisitor visitor(stream, automaton->getMaxDeadlockDistance(), m_showEmptyState);
        const ClosureAutomaton::StateSet& states = automaton->getStates();
        ClosureState::resetVisited(states.begin(), states.end());
        
        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const ClosureState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
    
    void RenderClosureAutomaton::operator()(const ClosureAutomaton::Ptr closureAutomaton, const RegionAutomaton::Ptr regionAutomaton, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        ClosureVisitor visitor(stream, closureAutomaton->getMaxDeadlockDistance(), m_showEmptyState, regionAutomaton);
        const ClosureAutomaton::StateSet& states = closureAutomaton->getStates();
        ClosureState::resetVisited(states.begin(), states.end());
        
        ClosureAutomaton::StateSet::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const ClosureState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
}
