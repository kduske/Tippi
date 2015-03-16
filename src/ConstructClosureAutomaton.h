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

#ifndef __Tippi__ConstructClosureAutomaton__
#define __Tippi__ConstructClosureAutomaton__

#include "SharedPointer.h"
#include "StringUtils.h"
#include "Closure.h"
#include "IntervalNet.h"
#include "IntervalNetState.h"

#include <iostream>

namespace Tippi {
    namespace Interval {
        class FiringRule;
        class Net;
    }
    
    struct ConstructClosureAutomaton {
    private:
        typedef enum {
            TransitionType_InputSend,
            TransitionType_InputRead,
            TransitionType_OutputSend,
            TransitionType_OutputRead,
            TransitionType_Internal
        } TransitionType;
        
        bool m_useAnonymousStateNames;
        
        typedef std::vector<TransitionType> TransitionTypes;
        TransitionTypes m_transitionTypes;
    public:
        typedef std::tr1::shared_ptr<Interval::Net> NetPtr;
        
        ConstructClosureAutomaton();
        void setUseAnonymousStateNames();
        ClosureAutomaton::Ptr operator()(const NetPtr net);
    private:
        void updateTransitionTypes(const NetPtr net);
        
        void buildAutomaton(const NetPtr net, ClosureAutomaton::Ptr automaton) const;
        
        void handleState(const NetPtr net,
                         const Interval::FiringRule& rule,
                         ClosureState* state,
                         ClosureAutomaton::Ptr automaton) const;
        
        ClosureEdge::EdgeType getEdgeType(const Interval::Transition* transition) const;
        
        void handleSuccessors(const NetPtr net,
                              const Interval::FiringRule& rule,
                              ClosureState* state,
                              const Interval::NetState::Set& successors,
                              const String& label,
                              ClosureEdge::EdgeType type,
                              ClosureAutomaton::Ptr automaton) const;
        
        bool isFinalState(const NetPtr net, const ClosureState* state) const;
        
        Interval::NetState::Set getSuccessorsForObservableTransition(const NetPtr net,
                                                                     const Interval::FiringRule& rule,
                                                                     const Interval::NetState::Set& states,
                                                                     const Interval::Transition* transition) const;
        
        Interval::NetState::Set getSuccessorsForTimeStep(const NetPtr net,
                                                         const Interval::FiringRule& rule,
                                                         const Interval::NetState::Set& states) const;
    };
}

#endif /* defined(__Tippi__ClosureAutomaton__) */
