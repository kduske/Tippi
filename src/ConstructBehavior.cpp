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

#include "ConstructBehavior.h"

#include "IntervalNetFiringRule.h"
#include "IntervalNet.h"

#include <cassert>

namespace Tippi {
    ConstructBehavior::ConstructBehavior(const bool createBoundViolationState) :
    m_createBoundViolationState(createBoundViolationState) {}

    Behavior::Ptr ConstructBehavior::operator()(const NetPtr net) const {
        Behavior::Ptr behavior(new Behavior());
        
        const Interval::NetState initialState = Interval::NetState::createInitialState(*net);
        BehaviorState* behState = behavior->createState(initialState);
        behavior->setInitialState(behState);

        const Interval::FiringRule rule(*net);
        handleState(net, rule, behState, behavior.get());
        
        return behavior;
    }

    void ConstructBehavior::handleState(const NetPtr net, const Interval::FiringRule& rule, BehaviorState* state, Behavior* behavior) const {
        assert(state != NULL);
        assert(behavior != NULL);
        
        const Interval::NetState& netState = state->getNetState();
        const Interval::Transition::List fireableTransitions = rule.getFireableTransitions(netState);
        Interval::Transition::List::const_iterator it, end;
        for (it = fireableTransitions.begin(), end = fireableTransitions.end(); it != end; ++it) {
            Interval::Transition* transition = *it;
            const Interval::NetState succNetState = rule.fireTransition(transition, netState);
            handleNetState(net, rule, state, succNetState, transition->getLabel(), behavior);
        }
        
        if (rule.canMakeTimeStep(netState)) {
            const Interval::NetState succNetState = rule.makeTimeStep(netState);
            handleNetState(net, rule, state, succNetState, "1", behavior);
        }
    }

    void ConstructBehavior::handleNetState(const NetPtr net, const Interval::FiringRule& rule, BehaviorState* state, const Interval::NetState& succNetState, const String& edgeLabel, Behavior* behavior) const {

        BehaviorState* succState = NULL;
        if (!succNetState.isBounded(*net)) {
            if (m_createBoundViolationState)
                succState = behavior->findOrCreateBoundViolationState();
        } else {
            std::pair<Behavior::State*, bool> result = behavior->findOrCreateState(succNetState);
            succState = result.first;

            if (result.second) {
                if (succNetState.isFinalMarking(*net)) {
                    succState->setFinal(true);
                    behavior->addFinalState(succState);
                }
                handleState(net, rule, succState, behavior);
            }
        }
        
        if (succState != NULL) {
            if (edgeLabel.empty())
                behavior->connectWithTauEdge(state, succState);
            else
                behavior->connectWithLabeledEdge(state, succState, edgeLabel);
        }
    }
}
