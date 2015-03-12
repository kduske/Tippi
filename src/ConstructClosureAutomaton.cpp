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

#include "ConstructClosureAutomaton.h"

#include "Closure.h"
#include "IntervalNetFiringRule.h"
#include "Exceptions.h"

#include <cassert>

namespace Tippi {
    ConstructClosureAutomaton::ConstructClosureAutomaton() :
    m_useAnonymousStateNames(false) {}
    
    void ConstructClosureAutomaton::setUseAnonymousStateNames() {
        m_useAnonymousStateNames = true;
    }

    ClosureAutomaton::Ptr ConstructClosureAutomaton::operator()(const NetPtr net) {
        updateTransitionTypes(net);
        
        ClosureAutomaton::Ptr automaton(new ClosureAutomaton());
        buildAutomaton(net, automaton);
        return automaton;
    }
    
    void ConstructClosureAutomaton::updateTransitionTypes(const NetPtr net) {
        const Interval::Transition::List& transitions = net->getTransitions();
        m_transitionTypes = TransitionTypes(transitions.size(), Internal);
        
        Interval::Transition::List::const_iterator tIt, tEnd;
        for (tIt = transitions.begin(), tEnd = transitions.end(); tIt != tEnd; ++tIt) {
            const Interval::Transition* transition = *tIt;
            const size_t index = transition->getIndex();
            const bool inputSend = transition->isInputSend();
            const bool inputRead = transition->isInputRead();
            const bool outputSend = transition->isOutputSend();
            const bool outputRead = transition->isOutputRead();
            
            if (!(!inputSend && !inputRead && !outputSend && !outputRead) &&
                !( inputSend  ^  inputRead  ^  outputSend  ^  outputRead))
                throw ClosureException("Transition '" + transition->getName() + "' is connected to more than one interface place");
            
            if (inputSend)
                m_transitionTypes[index] = InputSend;
            else if (inputRead)
                m_transitionTypes[index] = InputRead;
            else if (outputSend)
                m_transitionTypes[index] = OutputSend;
            else if (outputRead)
                m_transitionTypes[index] = OutputRead;
            else
                m_transitionTypes[index] = Internal;
        }
    }

    void ConstructClosureAutomaton::buildAutomaton(const NetPtr net, ClosureAutomaton::Ptr automaton) const {
        Interval::FiringRule rule(*net);
        const Interval::NetState initialState = Interval::NetState::createInitialState(*net);
        
        const ClResult initialResult = rule.buildClosure(initialState);
        if (initialResult.second) {
            const Closure closure(initialResult.first);
            ClosureState* initialState = automaton->createState(closure);
            automaton->setInitialState(initialState);
            handleState(net, rule, initialState, automaton);
        }
    }

    void ConstructClosureAutomaton::handleState(const NetPtr net,
                                                const Interval::FiringRule& rule,
                                                ClosureState* state,
                                                ClosureAutomaton::Ptr automaton) const {
        
        const Closure& closure = state->getClosure();

        const Interval::Transition::List& transitions = net->getTransitions();
        Interval::Transition::List::const_iterator it, end;
        for (it = transitions.begin(), end = transitions.end(); it != end; ++it) {
            const Interval::Transition* transition = *it;
            if (m_transitionTypes[transition->getIndex()] != Internal) {
                const Interval::NetState::Set successors = getSuccessorsForObservableTransition(net, rule, closure.getStates(), transition);
                const String& label = transition->getLabel();
                const ClosureEdge::Type type = getEdgeType(transition);
                
                assert((state->isEmpty() && successors.empty()) ||
                       !SetUtils::equals(closure.getStates(), successors));
                handleSuccessors(net, rule, state, successors, label, type, automaton);
            }
        }
        
        const Interval::NetState::Set successors = getSuccessorsForTimeStep(net, rule, closure.getStates());
        handleSuccessors(net, rule, state, successors, "1", ClosureEdge::Time, automaton);
    }
    
    ClosureEdge::Type ConstructClosureAutomaton::getEdgeType(const Interval::Transition* transition) const {
        switch (m_transitionTypes[transition->getIndex()]) {
            case InputSend:
                return ClosureEdge::InputSend;
            case InputRead:
                return ClosureEdge::InputRead;
            case OutputSend:
                return ClosureEdge::OutputSend;
            case OutputRead:
                return ClosureEdge::OutputRead;
            default:
                throw ClosureException("Unknown transition type");
        }
    }

    void ConstructClosureAutomaton::handleSuccessors(const NetPtr net,
                                                     const Interval::FiringRule& rule,
                                                     ClosureState* state,
                                                     const Interval::NetState::Set& successors,
                                                     const String& label,
                                                     const ClosureEdge::Type type,
                                                     ClosureAutomaton::Ptr automaton) const {
        typedef std::pair<ClosureState*, bool> ClosureStateResult;
        
        const ClResult succResult = rule.buildClosure(successors);
        if (succResult.second) {
            const Interval::NetState::Set& succClosureStates = succResult.first;
            const Closure succClosure(succClosureStates);
            const ClosureStateResult succStateResult = automaton->findOrCreateState(succClosure);
            ClosureState* succState = succStateResult.first;
            automaton->connectWithObservableEdge(state, succState, label, type);
            if (succStateResult.second) {
                if (isFinalState(net, succState)) {
                    succState->setFinal(true);
                    automaton->addFinalState(succState);
                }
                handleState(net, rule, succState, automaton);
            }
        }
    }
    
    bool ConstructClosureAutomaton::isFinalState(const NetPtr net, const ClosureState* state) const {
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
