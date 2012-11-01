//
//  ConstructBehavior.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "ConstructBehavior.h"

#include "Behavior.h"
#include "BehaviorState.h"
#include "BehaviorEdge.h"
#include "FiringRule.h"
#include "Net.h"
#include "NetTypes.h"
#include "NetState.h"

#include <stack>

namespace Tippi {
    ConstructBehavior::ConstructBehavior(const Net& net, Behavior& behavior, FiringRule& firingRule) :
    m_net(net),
    m_behavior(behavior),
    m_firingRule(firingRule) {}

    void ConstructBehavior::operator()() {
        typedef std::stack<BehaviorState*> BehaviorStateStack;
        
        const TransitionList& transitions = m_net.transitions();
        BehaviorStateStack unprocessedStates;
        
        NetState initialNetState = m_firingRule.initialState();
        BehaviorState* initialBehaviorState = m_behavior.createBehaviorState(initialNetState, m_net.isFinalPlaceMarking(initialNetState.placeMarking()));
        m_behavior.setInitialState(initialBehaviorState);
        unprocessedStates.push(initialBehaviorState);
        
        while (!unprocessedStates.empty()) {
            BehaviorState* currentState = unprocessedStates.top(); unprocessedStates.pop();
            
            unsigned int minTime = m_firingRule.minimumTime(currentState->netState());
            unsigned int maxTime = m_firingRule.maximumTime(currentState->netState());
            
            for (unsigned int time = minTime; time <= maxTime; time++) {
                NetState intermediateNetState = m_firingRule.passTime(currentState->netState(), time);
                for (unsigned int i = 0; i < transitions.size(); i++) {
                    Transition& transition = *transitions[i];
                    if (m_firingRule.isFireable(transition, intermediateNetState)) {
                        NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                        if (!nextNetState.placeMarking().violatesBound()) {
                            BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                            if (successorState == NULL) {
                                successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                unprocessedStates.push(successorState);
                            }
                            m_behavior.connect(currentState, successorState, time, transition);
                        }
                    }
                }
            }
        }
    }
}