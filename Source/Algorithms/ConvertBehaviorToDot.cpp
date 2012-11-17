//
//  ConvertBehaviorToDot.cpp
//  Tippi
//
//  Created by Kristian Duske on 26.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "ConvertBehaviorToDot.h"

#include "Behavior.h"
#include "BehaviorEdge.h"
#include "BehaviorState.h"
#include "GraphAlgorithms.h"
#include "Transition.h"

#include <sstream>

namespace Tippi {
    ConvertBehaviorToDot::ConvertBehaviorToDot(const Behavior& behavior, std::ostream& stream, const std::string& graphName) :
    m_behavior(behavior),
    m_stream(stream),
    m_graphName(graphName) {}

    std::string ConvertBehaviorToDot::printStateName(BehaviorState* state) {
        static unsigned int stateIndex = 0;
        
        std::stringstream nameStream;
        nameStream << "node" << stateIndex++;
        std::string name = nameStream.str();
        
        m_stream << "    " << name;
        m_stream << " [label=\"" << state->netState().placeMarking().asShortString() << "\\n" << state->netState().timeMarking().asShortString() << "\"";
        
        if (state->isFinal())
            m_stream << ", peripheries=2";
        
        m_stream << "]" << std::endl;
        return name;
    }

    void ConvertBehaviorToDot::visitState(BehaviorState* state) {
        StateNames::iterator nameIt = m_stateNames.find(state);
        if (nameIt == m_stateNames.end())
            m_stateNames[state] = printStateName(state);
    }
    
    void ConvertBehaviorToDot::visitEdge(BehaviorEdge* edge) {
        std::string& sourceName = m_stateNames[edge->source()];
        std::string& targetName = m_stateNames[edge->target()];
        m_stream << "    " << sourceName << " -> " << targetName << " [label=\"\\[" << edge->minTime() << ",";
        if (edge->maxTime() == Transition::Infinite)
            m_stream << "oo";
        else
            m_stream << edge->maxTime();
        m_stream << "\\]," << edge->transition().name() << "\"]" << std::endl;
    }

    void ConvertBehaviorToDot::operator()() {
        m_stream << "digraph " << m_graphName << " {" << std::endl;
        
        BehaviorState* initialState = m_behavior.initialState();
        if (initialState != NULL) {
            depthFirst<BehaviorState, BehaviorEdge>(initialState,
                                                    NodeFunctor<ConvertBehaviorToDot, BehaviorState>(this, &ConvertBehaviorToDot::visitState),
                                                    EdgeFunctor<ConvertBehaviorToDot, BehaviorEdge>(this, &ConvertBehaviorToDot::visitEdge));
        }
        
        m_stream << "}" << std::endl;
    }
}