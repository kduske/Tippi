/*
 Copyright (C) 2013 Kristian Duske
 
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

#include "Behavior.h"
#include "Net/IntervalNetFiringRule.h"
#include "Net/IntervalNet.h"

#include <cassert>

namespace Tippi {
    ConstructBehavior::BehPtr ConstructBehavior::operator()(const NetPtr net) const {
        BehPtr automaton(new Behavior::Automaton());
        
        const Interval::NetState initialState = Interval::NetState::createInitialState(*net);
        Behavior::State* behState = automaton->createState(initialState);
        automaton->setInitialState(behState);

        const Interval::FiringRule rule(*net);
        handleState(net, rule, behState, automaton.get());
        
        return automaton;
    }

    void ConstructBehavior::handleState(const NetPtr net, const Interval::FiringRule& rule, Behavior::State* state, Behavior::Automaton* automaton) const {
        assert(state != NULL);
        assert(automaton != NULL);
        
        const Interval::NetState& netState = state->getNetState();
        const Interval::Transition::List fireableTransitions = rule.getFireableTransitions(netState);
        Interval::Transition::List::const_iterator it, end;
        for (it = fireableTransitions.begin(), end = fireableTransitions.end(); it != end; ++it) {
            Interval::Transition* transition = *it;
            const Interval::NetState succNetState = rule.fireTransition(transition, netState);
            if (succNetState.isBounded(*net))
                handleNetState(net, rule, state, succNetState, transition->getName(), automaton);
        }
        
        if (rule.canMakeTimeStep(netState)) {
            const Interval::NetState succNetState = rule.makeTimeStep(netState);
            handleNetState(net, rule, state, succNetState, "1", automaton);
        }
    }

    void ConstructBehavior::handleNetState(const NetPtr net, const Interval::FiringRule& rule, Behavior::State* state, const Interval::NetState& succNetState, const String& edgeLabel, Behavior::Automaton* automaton) const {

        std::pair<Behavior::State*, bool> result = automaton->findOrCreateState(succNetState);
        Behavior::State* succState = result.first;
        automaton->connect(state, succState, edgeLabel);
        
        if (result.second) {
            if (succNetState.isFinalMarking(*net)) {
                succState->setFinal(true);
                automaton->addFinalState(succState);
            }
            handleState(net, rule, succState, automaton);
        }
    }
}
