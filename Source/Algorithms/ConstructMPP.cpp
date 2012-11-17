//
//  ConstructMPP.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "ConstructMPP.h"

#include "Behavior.h"
#include "BehaviorState.h"
#include "BehaviorEdge.h"
#include "MPP.h"
#include "Net.h"
#include "Transition.h"

#include <cassert>

namespace Tippi {
    ConstructMPP::ClosureEdge::ClosureEdge(const BehaviorEdge& externalEdge) :
    m_behaviorEdge(&externalEdge),
    m_minTime(m_behaviorEdge->minTime()),
    m_maxTime(m_behaviorEdge->maxTime()) {
        assert(!externalEdge.transition().internal());
    }
    
    ConstructMPP::ClosureEdge::ClosureEdge(const BehaviorEdge& internalEdge, const ClosureEdge& externalEdge) :
    m_behaviorEdge(&externalEdge.behaviorEdge()),
    m_minTime(internalEdge.minTime() + externalEdge.minTime()) {
        assert(internalEdge.transition().internal());
        assert(!externalEdge.behaviorEdge().transition().internal());
        
        if (internalEdge.maxTime() == Transition::Infinite || externalEdge.maxTime() == Transition::Infinite)
            m_maxTime = Transition::Infinite;
        else
            m_maxTime = internalEdge.maxTime() + externalEdge.maxTime();
        
    }

    ConstructMPP::Closure::Closure(const BehaviorState& initialState) {
        m_states.insert(&initialState);
    }

    void ConstructMPP::Closure::addClosure(const BehaviorEdge& viaInternalEdge, const Closure& closure) {
        assert(viaInternalEdge.transition().internal());

        const Closure::StateSet& closureStates = closure.states();
        m_states.insert(closureStates.begin(), closureStates.end());
        
        const Closure::EdgeSet& externalEdges = closure.edges();
        Closure::EdgeSet::const_iterator edgeIt, edgeEnd;
        for (edgeIt = externalEdges.begin(), edgeEnd = externalEdges.end(); edgeIt != edgeEnd; ++edgeIt) {
            const ClosureEdge& externalEdge = *edgeIt;
            m_edges.insert(ClosureEdge(viaInternalEdge, externalEdge));
        }
    }
    
    void ConstructMPP::Closure::addEdge(const BehaviorEdge& externalEdge) {
        m_edges.insert(ClosureEdge(externalEdge));
    }

    
    const ConstructMPP::Closure& ConstructMPP::buildClosure(const BehaviorState& behaviorState) {
        ClosureMap::const_iterator it = m_closures.find(&behaviorState);
        if (it != m_closures.end())
            return it->second;
        
        Closure closure(behaviorState);
        const BehaviorEdgeList& behaviorEdges = behaviorState.outgoingEdges();
        
        BehaviorEdgeList::const_iterator behEdgeIt, behEdgeEnd;
        for (behEdgeIt = behaviorEdges.begin(), behEdgeEnd = behaviorEdges.end(); behEdgeIt != behEdgeEnd; ++behEdgeIt) {
            const BehaviorEdge& behaviorEdge = **behEdgeIt;
            if (behaviorEdge.transition().internal()) {
                const BehaviorState& successorState = *behaviorEdge.target();
                const Closure& successorClosure = buildClosure(successorState);
                closure.addClosure(behaviorEdge, successorClosure);
            } else {
                closure.addEdge(behaviorEdge);
            }
        }
        
        ClosureMapInsertResult insertResult = m_closures.insert(ClosureMapEntry(&behaviorState, closure));
        assert(insertResult.second);
        return insertResult.first->second;
    }

    MPPState::DeadlockClass ConstructMPP::determineDeadlockClass(const BehaviorState& behaviorState) {
        DeadlockClassMap::const_iterator it = m_deadlockClasses.find(&behaviorState);
        if (it != m_deadlockClasses.end())
            return it->second;
        
        MPPState::DeadlockClass deadlockClass = MPPState::Undetermined;
        const BehaviorEdgeList& outgoingEdges = behaviorState.outgoingEdges();
        
        if (m_net.isFinalPlaceMarking(behaviorState.netState().placeMarking())) {
            deadlockClass = MPPState::NoDeadlock;
            m_deadlockClasses[&behaviorState] = deadlockClass;
            return deadlockClass;
        } else if (outgoingEdges.empty()) {
            deadlockClass = MPPState::Deadlock;
            m_deadlockClasses[&behaviorState] = deadlockClass;
            return deadlockClass;
        }
        
        MPPState::DeadlockClass classForInternallyReachableStates = MPPState::Undetermined;
        MPPState::DeadlockClass classForExternallyReachableStates = MPPState::Undetermined;
        
        BehaviorEdgeList::const_iterator edgeIt, edgeEnd;
        for (edgeIt = outgoingEdges.begin(), edgeEnd = outgoingEdges.end(); edgeIt != edgeEnd; ++edgeIt) {
            const BehaviorEdge& outgoingEdge = **edgeIt;
            const BehaviorState& targetState = static_cast<const BehaviorState&>(*outgoingEdge.target());
            MPPState::DeadlockClass targetClass = determineDeadlockClass(targetState);
            assert(targetClass != MPPState::Undetermined);
            
            if (outgoingEdge.transition().internal()) {
                switch (classForInternallyReachableStates) {
                    case MPPState::Undetermined:
                        classForInternallyReachableStates = targetClass;
                        break;
                    case MPPState::Deadlock:
                        // remains a deadlock
                        break;
                    case MPPState::Controllable:
                        if (targetClass == MPPState::Deadlock)
                            classForInternallyReachableStates = MPPState::Deadlock;
                        break;
                    case MPPState::NoDeadlock:
                        classForInternallyReachableStates = targetClass;
                        break;
                }
            } else {
                switch (classForExternallyReachableStates) {
                    case MPPState::Undetermined:
                        classForExternallyReachableStates = targetClass;
                        break;
                    case MPPState::Deadlock:
                        if (targetClass != MPPState::Deadlock)
                            classForExternallyReachableStates = MPPState::Controllable;
                        break;
                    case MPPState::Controllable:
                        // remains controllable
                        break;
                    case MPPState::NoDeadlock:
                        if (targetClass != MPPState::NoDeadlock)
                            classForExternallyReachableStates = MPPState::Controllable;
                        break;
                }
            }
        }

        if (classForExternallyReachableStates == MPPState::Undetermined)
            deadlockClass = classForInternallyReachableStates;
        else if (classForInternallyReachableStates == MPPState::Undetermined)
            deadlockClass = classForExternallyReachableStates;
        else if (classForExternallyReachableStates == MPPState::Deadlock &&
                 classForInternallyReachableStates == MPPState::Deadlock)
            deadlockClass = MPPState::Deadlock;
        else if (classForExternallyReachableStates == MPPState::NoDeadlock &&
                 classForInternallyReachableStates == MPPState::NoDeadlock)
            deadlockClass = MPPState::NoDeadlock;
        else
            deadlockClass = MPPState::Controllable;
        
        m_deadlockClasses[&behaviorState] = deadlockClass;
        return deadlockClass;
    }

    MPPState* ConstructMPP::processState(const BehaviorState& behaviorState, bool initialState) {
        MPPState* mppState = m_mpp.mppState(behaviorState.netState());
        if (mppState != NULL)
            return mppState;
        
        bool externallyReachable = false;
        const BehaviorEdgeList& incomingEdges = behaviorState.incomingEdges();
        BehaviorEdgeList::const_iterator edgeIt, edgeEnd;
        for (edgeIt = incomingEdges.begin(), edgeEnd = incomingEdges.end(); edgeIt != edgeEnd && !externallyReachable; ++edgeIt) {
            const BehaviorEdge& edge = **edgeIt;
            externallyReachable = !edge.transition().internal();
        }
        
        if (!externallyReachable && !initialState)
            return NULL;
        
        const Closure& closure = buildClosure(behaviorState);
        MPPState::DeadlockClass deadlockClass = determineDeadlockClass(behaviorState);
        
        const BehaviorEdgeList& outgoingEdges = behaviorState.outgoingEdges();
        if (outgoingEdges.empty())
            return m_mpp.createMPPState(deadlockClass, behaviorState);
        
        mppState = m_mpp.createMPPState(deadlockClass, behaviorState);
        
        const Closure::StateSet& closureStates = closure.states();
        Closure::StateSet::const_iterator clStateIt, clStateEnd;
        for (clStateIt = closureStates.begin(), clStateEnd = closureStates.end(); clStateIt != clStateEnd; ++clStateIt) {
            const BehaviorState& reachableState = **clStateIt;
            mppState->addNetState(reachableState.netState(), reachableState.isFinal());
        }
        
        // create edges to all successor MPP states
        const Closure::EdgeSet& externalEdges = closure.edges();
        Closure::EdgeSet::const_iterator clEdgeIt, clEdgeEnd;
        for (clEdgeIt = externalEdges.begin(), clEdgeEnd = externalEdges.end(); clEdgeIt != clEdgeEnd; ++clEdgeIt) {
            const ClosureEdge& externalEdge = *clEdgeIt;

            const BehaviorState& targetBehaviorState = *externalEdge.behaviorEdge().target();
            MPPState* targetMPPState = processState(targetBehaviorState, false);
            const Transition& transition = externalEdge.behaviorEdge().transition();

            m_mpp.connect(mppState, targetMPPState, externalEdge.minTime(), externalEdge.maxTime(), transition);
        }
        
        return mppState;
    }

    ConstructMPP::ConstructMPP(const Net& net, const Behavior& behavior, MPP& mpp) :
    m_net(net),
    m_behavior(behavior),
    m_mpp(mpp) {}

    void ConstructMPP::operator()() {
        MPPState* initialState = processState(*m_behavior.initialState(), true);
        assert(initialState != NULL);
        m_mpp.setInitialState(initialState);
    }
}