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

#include "ConstructClosureAutomaton.h"

#include "Closure.h"
#include "Net/IntervalNetFiringRule.h"
#include "Exceptions.h"

#include <cassert>

namespace Tippi {
    ConstructClosureAutomaton::ClPtr ConstructClosureAutomaton::operator()(const NetPtr net) const {
        const Interval::Transition::List observableTransitions = getObservableTransitions(net);
        
        Interval::FiringRule rule(*net);
        ClPtr automaton(new ClAutomaton());
        
        const Interval::NetState initialState = Interval::NetState::createInitialState(*net);
        const ClResult initialResult = rule.buildClosure(initialState);
        if (initialResult.second) {
            const Closure closure(initialResult.first);
            ClState* initialState = automaton->createState(closure);
            automaton->setInitialState(initialState);
            handleState(net,
                        rule,
                        initialState,
                        observableTransitions,
                        automaton);
        }
        
        return automaton;
    }
    
    Interval::Transition::List ConstructClosureAutomaton::getObservableTransitions(const NetPtr net) const {
        Interval::Transition::List transitions;
        const Interval::Place::List& places = net->getPlaces();
        Interval::Place::List::const_iterator it, end;
        for (it = places.begin(), end = places.end(); it != end; ++it) {
            const Interval::Place* place = *it;
            if (place->isInputPlace() || place->isOutputPlace()) {
                const Interval::TransitionToPlace::List& incoming = place->getIncoming();
                const Interval::PlaceToTransition::List& outgoing = place->getOutgoing();
                if (!incoming.size() == 1)
                    throw ClosureException("Interface place '" + place->getName() + "' must have exactly one transition in its preset");
                if (!outgoing.size() == 1)
                    throw ClosureException("Interface place '" + place->getName() + "' must have exactly one transition in its preset");
                
                Interval::Transition* preTransition = incoming[0]->getSource();
                Interval::Transition* postTransition = outgoing[0]->getTarget();
                transitions.push_back(preTransition);
                transitions.push_back(postTransition);
            }
        }
        return transitions;
    }
    
    void ConstructClosureAutomaton::handleState(const NetPtr net,
                                                const Interval::FiringRule& rule,
                                                ClState* state,
                                                const Interval::Transition::List& observableTransitions,
                                                ClPtr automaton) const {
        
        const Closure& closure = state->getClosure();
        Interval::Transition::List::const_iterator it, end;
        for (it = observableTransitions.begin(), end = observableTransitions.end(); it != end; ++it) {
            const Interval::Transition* transition = *it;
            const Interval::NetState::Set successors = getSuccessorsForObservableTransition(net, rule, closure.getStates(), transition);
            handleSuccessors(net, rule, state, successors, transition->getLabel(), observableTransitions, automaton);
        }
        
        const Interval::NetState::Set successors = getSuccessorsForTimeStep(net, rule, closure.getStates());
        handleSuccessors(net, rule, state, successors, "1", observableTransitions, automaton);
    }
    
    void ConstructClosureAutomaton::handleSuccessors(const NetPtr net,
                                                     const Interval::FiringRule& rule,
                                                     ClState* state,
                                                     const Interval::NetState::Set& successors,
                                                     const String& label,
                                                     const Interval::Transition::List& observableTransitions,
                                                     ClPtr automaton) const {
        typedef std::pair<ClState*, bool> ClStateResult;
        
        const ClResult succResult = rule.buildClosure(successors);
        if (succResult.second) {
            const Interval::NetState::Set& succClStates = succResult.first;
            const Closure succClosure(succClStates);
            const ClStateResult succStateResult = automaton->findOrCreateState(succClosure);
            ClState* succState = succStateResult.first;
            automaton->connect(state, succState, label);
            if (succStateResult.second) {
                if (isFinalState(net, succState)) {
                    succState->setFinal(true);
                    automaton->addFinalState(succState);
                }
                handleState(net, rule, succState, observableTransitions, automaton);
            }
        }
    }
    
    bool ConstructClosureAutomaton::isFinalState(const NetPtr net, const ClState* state) const {
        const Marking::List& finalMarkings = net->getFinalMarkings();
        const Interval::NetState::Set& states = state->getClosure().getStates();
        
        Marking::List::const_iterator mIt = finalMarkings.begin();
        const Marking::List::const_iterator mEnd = finalMarkings.end();
        Interval::NetState::Set::const_iterator sIt = states.begin();
        const Interval::NetState::Set::const_iterator sEnd = states.end();
        
        while (mIt != mEnd && sIt != sEnd) {
            const Marking& marking = *mIt;
            const Interval::NetState& netState = *sIt;
            const int cmp = netState.comparePlaceMarking(marking);
            if (cmp == 0)
                return true;
            if (cmp < 0) // netState < placeMarking
                ++sIt;
            else if (cmp > 0) // placeMarking < netState
                ++mIt;
        }
        return false;
    }

    Interval::NetState::Set ConstructClosureAutomaton::getSuccessorsForObservableTransition(const NetPtr net,
                                                                                            const Interval::FiringRule& rule,
                                                                                            const Interval::NetState::Set& states,
                                                                                            const Interval::Transition* transition) const {
        
        Interval::NetState::Set successors;
        
        Interval::NetState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const Interval::NetState& state = *it;
            if (rule.isFireable(transition, state)) {
                const Interval::NetState successor = rule.fireTransition(transition, state);
                successors.insert(successor);
            }
        }
        
        return successors;
    }
    
    Interval::NetState::Set ConstructClosureAutomaton::getSuccessorsForTimeStep(const NetPtr net,
                                                                                const Interval::FiringRule& rule,
                                                                                const Interval::NetState::Set& states) const {
        Interval::NetState::Set successors;
        
        Interval::NetState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const Interval::NetState& state = *it;
            if (rule.canMakeTimeStep(state)) {
                const Interval::NetState successor = rule.makeTimeStep(state);
                successors.insert(successor);
            }
        }
        
        return successors;
    }
}
