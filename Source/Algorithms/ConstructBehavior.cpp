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
            const NetState& currentNetState = currentState->netState();
            const PlaceMarking& currentPlaceMarking = currentNetState.placeMarking();
            const TimeMarking& currentTimeMarking = currentNetState.timeMarking();

            size_t enabledTransitionCount = 0;
            size_t finiteTransitionCount = 0;
            size_t infiniteTransitionCount = 0;
            unsigned int maxTime = std::numeric_limits<unsigned int>::max();
            unsigned int timeUntilAllFireable = 0;
            
            TransitionList::const_iterator trIt, trEnd;
            for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                const Transition& transition = **trIt;
                if (m_firingRule.isEnabled(transition, currentNetState)) {
                    enabledTransitionCount++;
                    unsigned int eft = transition.earliestFiringTime();
                    unsigned int lft = transition.latestFiringTime();
                    unsigned int time = currentTimeMarking[transition];
                    if (lft == Transition::Infinite) {
                        infiniteTransitionCount++;
                    } else {
                        finiteTransitionCount++;
                        maxTime = std::min(maxTime, lft - time);
                    }

                    unsigned int timeUntilFireable = eft > time ? eft - time : 0;
                    timeUntilAllFireable = std::max(timeUntilAllFireable, timeUntilFireable);
                }
            }
            
            // are all transitions disabled?
            if (enabledTransitionCount == 0)
                continue;
            
            if (enabledTransitionCount == infiniteTransitionCount) {
                // all enabled transitions have an infinite latest firing time
                for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                    const Transition& transition = **trIt;
                    if (m_firingRule.isEnabled(transition, currentNetState)) {
                        unsigned int eft = transition.earliestFiringTime();
                        unsigned int time = currentTimeMarking[transition];
                        unsigned int passTime = eft > time ? eft - time : 0;
                        
                        NetState intermediateNetState = m_firingRule.passTime(currentNetState, passTime);
                        assert(m_firingRule.isFireable(transition, intermediateNetState));
                        
                        NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                        if (!nextNetState.placeMarking().violatesBound()) {
                            BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                            if (successorState == NULL) {
                                successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                unprocessedStates.push(successorState);
                            }
                            m_behavior.connect(currentState, successorState, passTime, Transition::Infinite, transition);
                        }
                    }
                }
            } else {
                for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                    const Transition& transition = **trIt;
                    if (m_firingRule.isEnabled(transition, currentNetState)) {
                        unsigned int eft = transition.earliestFiringTime();
                        unsigned int lft = transition.latestFiringTime();
                        unsigned int time = currentTimeMarking[transition];
                        unsigned minTime = eft > time ? eft - time : 0;
                        
                        if (finiteTransitionCount == 1 && transition.latestFiringTime() != Transition::Infinite) {
                            // only case in which there are parallel edges which can be merged because the successor
                            // state enables at most transitions with infinite latest firing time
                        } else {
                        }
                    }
                }
            }
            
            
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
