//
//  ConvertMPPToDot.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "ConvertMPPToDot.h"

#include "GraphAlgorithms.h"
#include "MPP.h"
#include "NetState.h"
#include "PlaceMarking.h"
#include "TimeMarking.h"

#include <cassert>

namespace Tippi {
    std::string ConvertMPPToDot::printStateName(const MPPState& state) {
        static unsigned int stateIndex = 0;
        
        std::stringstream nameStream;
        nameStream << "node" << stateIndex++;
        std::string name = nameStream.str();
        
        m_stream << "    " << name << " [label=\"";
        
        const NetStateSet& netStates = state.netStates();
        NetStateSet::const_iterator stateIt, stateEnd;
        for (stateIt = netStates.begin(), stateEnd = netStates.end(); stateIt != stateEnd; ++stateIt) {
            const NetState& netState = *stateIt;
            m_stream << netState.placeMarking().asShortString() << "\\n" << netState.timeMarking().asShortString() << "\\n";
        }
        
        m_stream << "\"";
        if (state.isFinal())
            m_stream << ", peripheries=2";
        
        if (state.deadlockClass() == MPPState::Deadlock)
            m_stream << ", color=red";
        else if (state.deadlockClass() == MPPState::Controllable)
            m_stream << ", color=yellow";
        else if (state.deadlockClass() == MPPState::NoDeadlock)
            m_stream << ", color=green";

        m_stream << "]" << std::endl;
        return name;
    }
    
    void ConvertMPPToDot::visitState(MPPState* state) {
        StateNames::iterator nameIt = m_stateNames.find(state);
        if (nameIt == m_stateNames.end())
            m_stateNames[state] = printStateName(*state);
    }
    
    void ConvertMPPToDot::visitEdge(MPPEdge* edge) {
        std::string& sourceName = m_stateNames[edge->source()];
        std::string& targetName = m_stateNames[edge->target()];
        m_stream << "    " << sourceName << " -> " << targetName << " [label=\"\\[" << edge->minTime() << ",";
        if (edge->maxTime() == Transition::Infinite)
            m_stream << "oo";
        else
            m_stream << edge->maxTime();
        m_stream << "\\]," << edge->transition().name() << "\"]" << std::endl;
    }

    ConvertMPPToDot::ConvertMPPToDot(const MPP& mpp, std::ostream& stream, const std::string& graphName) :
    m_mpp(mpp),
    m_stream(stream),
    m_graphName(graphName) {}
    
    void ConvertMPPToDot::operator()() {
        m_stream << "digraph " << m_graphName << " {" << std::endl;
        
        MPPState* initialState = m_mpp.initialState();
        if (initialState != NULL) {
            depthFirst<MPPState, MPPEdge>(initialState,
                                          NodeFunctor<ConvertMPPToDot, MPPState>(this, &ConvertMPPToDot::visitState),
                                          EdgeFunctor<ConvertMPPToDot, MPPEdge>(this, &ConvertMPPToDot::visitEdge));
        }
        
        m_stream << "}" << std::endl;
    }
}
