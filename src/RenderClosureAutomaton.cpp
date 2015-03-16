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
    RenderClosureAutomaton::RenderClosureAutomaton(std::ostream& stream, const bool showEmptyState, const bool showSCCs) :
    m_stream(stream),
    m_showEmptyState(showEmptyState),
    m_showSCCs(showSCCs) {}

    void RenderClosureAutomaton::operator()(const ClosureAutomaton* automaton) {
        m_stream << "digraph {" << std::endl;
        
        if (m_showSCCs && automaton->getInitialState() != NULL) {
            typedef StronglyConnectedComponents<const ClosureState> SCC;
            SCC scc;
            scc.visitNode(automaton->getInitialState());
            
            const SCC::ComponentList& components = scc.components();
            size_t index = 0;
            SCC::ComponentList::const_iterator cIt, cEnd;
            for (cIt = components.begin(), cEnd = components.end(); cIt != cEnd; ++cIt) {
                const SCC::Component& component = *cIt;
                printComponent(component, ++index, automaton->getMaxDeadlockDistance());
            }
        } else {
            const ClosureAutomaton::StateSet& states = automaton->getStates();
            ClosureAutomaton::StateSet::const_iterator it, end;
            for (it = states.begin(), end = states.end(); it != end; ++it) {
                const ClosureState* state = *it;
                printState(state, automaton->getMaxDeadlockDistance());
            }
        }
        
        const ClosureAutomaton::EdgeSet edges = automaton->getEdges();
        ClosureAutomaton::EdgeSet::const_iterator eIt, eEnd;
        for (eIt = edges.begin(), eEnd = edges.end(); eIt != eEnd; ++eIt) {
            const ClosureEdge* edge = *eIt;
            printEdge(edge);
        }
        
        m_stream << "}" << std::endl;
    }

    void RenderClosureAutomaton::printComponent(const SCC::Component& component, const size_t index, const size_t maxDeadlockDistance) {
        
        if (m_showEmptyState || component.size() != 1 || !component.front()->isEmpty()) {
            m_stream << "subgraph cluster_" << index << " {" << std::endl;
            
            SCC::Component::const_iterator sIt, sEnd;
            for (sIt = component.begin(), sEnd = component.end(); sIt != sEnd; ++sIt) {
                const ClosureState* state = *sIt;
                printState(state, maxDeadlockDistance);
            }
            
            m_stream << "}" << std::endl;
        }
    }
    
    void RenderClosureAutomaton::printState(const ClosureState* state, const size_t maxDeadlockDistance) {
        if (m_showEmptyState || !state->isEmpty()) {
            m_stream << "" << state->getId() << " [";
            printAttribute("label", state->asString("\n", "\n"));
            m_stream << ",";
            printAttribute("shape", "ellipse");
            if (state->isFinal()) {
                m_stream << ",";
                printAttribute("peripheries", "2");
            }
            
            const size_t distance = state->getDeadlockDistance();
            if (!state->isReachable()) {
                m_stream << ",";
                printAttribute("style", "filled");
                m_stream << ",";
                printColorAttribute("fillcolor", 0, 255, 0);
            } else if (distance > 0 && maxDeadlockDistance > 0) {
                const float d = distance;
                const float m = maxDeadlockDistance;
                const size_t gb = static_cast<size_t>(d / m * 255.0f);
                m_stream << ",";
                printAttribute("style", "filled");
                m_stream << ",";
                printColorAttribute("fillcolor", 255, gb, gb);
            }
            m_stream << "];" << std::endl;
        }
    }
    
    void RenderClosureAutomaton::printEdge(const ClosureEdge* edge) {
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
    
    void RenderClosureAutomaton::printAttribute(const String& name, const String& value) {
        m_stream << name << "=\"" << value << "\"";
    }
    
    void RenderClosureAutomaton::printColorAttribute(const String& name, size_t r, size_t g, size_t b) {
        m_stream << name << "=\"#";
        m_stream << std::hex;
        m_stream << std::setfill('0') << std::setw(2) << r;
        m_stream << std::setfill('0') << std::setw(2) << g;
        m_stream << std::setfill('0') << std::setw(2) << b;
        m_stream << "\"";
        m_stream << std::dec;
    }
}
