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
            const TimeMarking& currentTimeMarking = currentNetState.timeMarking();

            size_t enabledTransitionCount = 0;
            size_t finiteTransitionCount = 0;
            size_t infiniteTransitionCount = 0;
            unsigned int timeUntilFirstCanFire = std::numeric_limits<unsigned int>::max();
            unsigned int timeUntilAllCanFire = 0;
            unsigned int timeUntilFirstMustFire = std::numeric_limits<unsigned int>::max();
            
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
                        unsigned int timeUntilMustFire = lft - time;
                        timeUntilFirstMustFire = std::min(timeUntilFirstMustFire, timeUntilMustFire);
                    }

                    unsigned int timeUntilCanFire = eft > time ? eft - time : 0;
                    timeUntilFirstCanFire = std::min(timeUntilFirstCanFire, timeUntilCanFire);
                    timeUntilAllCanFire = std::max(timeUntilAllCanFire, timeUntilCanFire);
                }
            }
            
            // are all transitions disabled?
            if (enabledTransitionCount == 0)
                continue;
            
            if (enabledTransitionCount == infiniteTransitionCount) {
                // all enabled transitions have an infinite latest firing time
                // first, create successor states for all steps until all transitions have become firable because the
                // time marking of a transition that is enabled but not yet firable will still change
                // then, create another step to represent time passing util infinity at the current state, because once
                // all transitions have become fireable, their time marking will not change any more
                for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                    const Transition& transition = **trIt;
                    if (m_firingRule.isEnabled(transition, currentNetState)) {
                        for (unsigned int time = timeUntilFirstCanFire; time < timeUntilAllCanFire; time++) {
                            NetState intermediateNetState = m_firingRule.passTime(currentNetState, time);
                            if (m_firingRule.isFireable(transition, intermediateNetState)) {
                                
                                NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                                if (!nextNetState.placeMarking().violatesBound()) {
                                    BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                                    if (successorState == NULL) {
                                        successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                        unprocessedStates.push(successorState);
                                    }
                                    m_behavior.connect(currentState, successorState, time, time, transition);
                                }
                            }
                        }
                        
                        NetState intermediateNetState = m_firingRule.passTime(currentNetState, timeUntilAllCanFire);
                        assert(m_firingRule.isFireable(transition, intermediateNetState));
                        
                        NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                        if (!nextNetState.placeMarking().violatesBound()) {
                            BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                            if (successorState == NULL) {
                                successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                unprocessedStates.push(successorState);
                            }
                            m_behavior.connect(currentState, successorState, timeUntilAllCanFire, Transition::Infinite, transition);
                        }
                    }
                }
            } else if (finiteTransitionCount == 1) {
                // first, create single steps for all enabled transitions until all transitions become fireable
                // then, create single steps for all enabled transitions except for the finite transition until the
                // finite transition must fire
                // create a compound step for the finite transition until it must fire
                
                for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                    const Transition& transition = **trIt;
                    if (m_firingRule.isEnabled(transition, currentNetState)) {
                        for (unsigned int time = timeUntilFirstCanFire; time < timeUntilAllCanFire; time++) {
                            NetState intermediateNetState = m_firingRule.passTime(currentNetState, time);
                            if (m_firingRule.isFireable(transition, intermediateNetState)) {
                                NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                                if (!nextNetState.placeMarking().violatesBound()) {
                                    BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                                    if (successorState == NULL) {
                                        successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                        unprocessedStates.push(successorState);
                                    }
                                    m_behavior.connect(currentState, successorState, time, time, transition);
                                }
                            }
                        }
                        
                        if (transition.latestFiringTime() == Transition::Infinite) {
                            for (unsigned int time = timeUntilAllCanFire; time <= timeUntilFirstMustFire; time++) {
                                NetState intermediateNetState = m_firingRule.passTime(currentNetState, time);
                                assert(m_firingRule.isFireable(transition, intermediateNetState));
                                
                                NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                                if (!nextNetState.placeMarking().violatesBound()) {
                                    BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                                    if (successorState == NULL) {
                                        successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                        unprocessedStates.push(successorState);
                                    }
                                    m_behavior.connect(currentState, successorState, time, time, transition);
                                }
                            }
                        } else {
                            NetState intermediateNetState = m_firingRule.passTime(currentNetState, timeUntilAllCanFire);
                            assert(m_firingRule.isFireable(transition, intermediateNetState));
                            NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                            if (!nextNetState.placeMarking().violatesBound()) {
                                BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                                if (successorState == NULL) {
                                    successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                    unprocessedStates.push(successorState);
                                }
                                m_behavior.connect(currentState, successorState, timeUntilAllCanFire, timeUntilFirstMustFire, transition);
                            }
                        }
                    }
                }
            } else {
                // create single steps for all enabled transitions
                for (trIt = transitions.begin(), trEnd = transitions.end(); trIt != trEnd; ++trIt) {
                    const Transition& transition = **trIt;
                    if (m_firingRule.isEnabled(transition, currentNetState)) {
                        for (unsigned int time = timeUntilFirstCanFire; time <= timeUntilFirstMustFire; time++) {
                            NetState intermediateNetState = m_firingRule.passTime(currentNetState, time);
                            if (m_firingRule.isFireable(transition, intermediateNetState)) {
                                NetState nextNetState = m_firingRule.fire(transition, intermediateNetState);
                                if (!nextNetState.placeMarking().violatesBound()) {
                                    BehaviorState* successorState = m_behavior.behaviorState(nextNetState);
                                    if (successorState == NULL) {
                                        successorState = m_behavior.createBehaviorState(nextNetState, m_net.isFinalPlaceMarking(nextNetState.placeMarking()));
                                        unprocessedStates.push(successorState);
                                    }
                                    m_behavior.connect(currentState, successorState, time, time, transition);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
