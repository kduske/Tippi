//
//  FiringRule.cpp
//  Tippi
//
//  Created by Kristian Duske on 17.09.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "FiringRule.h"

#include "Net.h"
#include "NetState.h"
#include "Transition.h"

namespace Tippi {
    bool FiringRule::checkEnabled(const Transition& transition, const PlaceMarking& marking) const {
        bool enabled = true;
        
        const Transition::IncomingEdgeList& incoming = transition.incomingEdges();
        for (unsigned int i = 0; i < incoming.size() && enabled; i++) {
            PlaceToTransition* arc = incoming[i];
            const Place* place = arc->source();
            enabled = (marking[*place] >= arc->multiplicity());
        }

        /* don't check for bound violations
        const Transition::OutgoingEdgeList& outgoing = transition.outgoingArcs();
        for (unsigned int i = 0; i < outgoing.size() && enabled; i++) {
            TransitionToPlace* arc = outgoing[i];
            Place* place = arc->target();
            enabled = (marking[*place] + arc->multiplicity() <= place->bound());
        }
         */
        
        return enabled;
    }
    

    FiringRule::FiringRule(Net& net) :
    m_net(net) {}

    
    TimeMarking FiringRule::initialTimeMarking() const {
        const TransitionList& transitions = m_net.transitions();
        TimeMarking timeMarking(transitions);
        TransitionList::const_iterator it, end;
        
        for (it = transitions.begin(), end = transitions.end(); it != end; ++it) {
            const Transition* transition = *it;
            if (checkEnabled(*transition, m_net.initialPlaceMarking()))
                timeMarking[*transition] = 0;
            else
                timeMarking[*transition] = TimeMarking::Disabled;
        }
        
        return timeMarking;
    }
    
    NetState FiringRule::initialState() const {
        return NetState(m_net.initialPlaceMarking(), initialTimeMarking());
    }

    bool FiringRule::isEnabled(const Transition& transition, const NetState& state) const {
        return state.timeMarking()[transition] != TimeMarking::Disabled;
    }

    bool FiringRule::isFireable(const Transition& transition, const NetState& state) const {
        /*
        if (m_net.isFinalPlaceMarking(state.placeMarking()))
            return false;
        */

        unsigned int marking = state.timeMarking()[transition];
        return marking != TimeMarking::Disabled && marking >= transition.earliestFiringTime();
    }
    
    TransitionList FiringRule::fireableTransitions(const NetState& state) const {
        const TransitionList& transitions = m_net.transitions();
        TransitionList result;
        
        for (unsigned int i = 0; i < transitions.size(); i++) {
            Transition* transition = transitions[i];
            if (isFireable(*transition, state))
                result.push_back(transition);
        }

        return result;
    }
    
    unsigned int FiringRule::minimumTime(const NetState& state) const {
        const TimeMarking& timeMarking = state.timeMarking();
        unsigned int time = std::numeric_limits<unsigned int>::max();
        
        const TransitionList& transitions = m_net.transitions();
        for (unsigned int i = 0; i < transitions.size(); i++) {
            const Transition& transition = *transitions[i];
            unsigned int marking = timeMarking[transition];
            if (marking != TimeMarking::Disabled) {
                unsigned int remaining = transition.earliestFiringTime() > marking ? transition.earliestFiringTime() - marking : 0;
                time = std::min(time, remaining);
            }
        }
        
        if (time == std::numeric_limits<unsigned int>::max()) // all transitions are disabled
            return 0;
        return time;
    }
    
    unsigned int FiringRule::maximumTime(const NetState& state) const {
        const TimeMarking& timeMarking = state.timeMarking();
        unsigned int maxUntilEft = 0;
        unsigned int maxTime = std::numeric_limits<unsigned int>::max();
        
        const TransitionList& transitions = m_net.transitions();
        for (unsigned int i = 0; i < transitions.size(); i++) {
            const Transition& transition = *transitions[i];
            unsigned int marking = timeMarking[transition];
            if (marking != TimeMarking::Disabled) {
                if (transition.latestFiringTime() != Transition::Infinite)
                    maxTime = std::min(maxTime, transition.latestFiringTime() > marking ? transition.latestFiringTime() - marking : 0);
                else
                    maxUntilEft = std::max(maxUntilEft, transition.earliestFiringTime() > marking ? transition.earliestFiringTime() - marking : 0);
            }
        }
        
        if (maxTime == std::numeric_limits<unsigned int>::max()) // all transitions with finite end times are disabled
            return maxUntilEft; // in this case, we must wait until all enabled transitions become fireable
        
        return maxTime;
    }

    NetState FiringRule::passTime(const NetState& state, unsigned int time) const {
        TimeMarking timeMarking = state.timeMarking();
        
        const TransitionList& transitions = m_net.transitions();
        for (unsigned int i = 0; i < transitions.size(); i++) {
            const Transition& transition = *transitions[i];
            unsigned int marking = timeMarking[transition];
            
            if (isEnabled(transition, state)) {
                /*
                if (transition.latestFiringTime() == Transition::Infinity) {
                    if (marking < transition.earliestFiringTime())
                        timeMarking[transition] = std::min(transition.earliestFiringTime(), marking + time);
                } else {
                    assert(marking + time <= transition.latestFiringTime());
                    timeMarking[transition] = marking + time;
                }
                 */
                timeMarking[transition] = marking + time;
            }
        }
        
        return NetState(state.placeMarking(), timeMarking);
    }

    NetState FiringRule::fire(const Transition& transition, const NetState& state) const {
        const Transition::IncomingEdgeList& incoming = transition.incomingEdges();
        const Transition::OutgoingEdgeList& outgoing = transition.outgoingEdges();
        
        const PlaceMarking& oldPlaceMarking = state.placeMarking();
        const TimeMarking& oldTimeMarking = state.timeMarking();
        
        PlaceMarking newPlaceMarking(oldPlaceMarking);
        TimeMarking newTimeMarking(oldTimeMarking);
        
        // 1. remove tokens from each preset place
        //    1. reset the clocks of all concurrent transitions (transitions in the postset of the preset places)
        //    2. check whether any transition in the postset of the preset place was enabled and becomes disabled
        //       because the place does not contain enough tokens anymore
        //    3. check whether any transition in the preset of the preset place was disabled because its firing would
        //       have violated the preset place's bound, and enable it if all other conditions are met
        // 2. add tokens from each postset place
        //    1. check whether any transition in the postset of the preset place becomes enabled because the place now
        //       containts sufficient tokens, and enable it if this is the case
        //    2. check whether any transition in the preset of the preset places becomes disabled because its firing
        //       would violate the place's bounds
        
        bool remainEnabled = true;
        for (unsigned int i = 0; i < incoming.size(); i++) {
            PlaceToTransition* incomingArc = incoming[i];
            const Place* place = incomingArc->source();
            const unsigned int marking = newPlaceMarking[*place];
            const unsigned int removedTokens = incomingArc->multiplicity();
            assert(marking >= removedTokens);
            const unsigned int newMarking = marking - removedTokens;
            newPlaceMarking[*place] = newMarking;
            remainEnabled &= (newMarking >= incomingArc->multiplicity());
            
            // every concurrent enabled transition has its clock reset
            // also set the time marking of concurrent transitions to Disabled if they become so
            const Place::OutgoingEdgeList& placeOutgoing = place->outgoingEdges();
            for (unsigned int j = 0; j < placeOutgoing.size(); j++) {
                PlaceToTransition* placeOutgoingArc = placeOutgoing[j];
                const Transition* concurrent = placeOutgoingArc->target();
                
                if (oldTimeMarking[*concurrent] != TimeMarking::Disabled) { // the transition was enabled
                    if (checkEnabled(*concurrent, newPlaceMarking)) // it remains enabled
                        newTimeMarking[*concurrent] = 0; // reset its clock
                    else // it's no longer enabled
                        newTimeMarking[*concurrent] = TimeMarking::Disabled; // disable it
                }
            }
            
            /* don't check for bound violations
            // for each place from which tokens are removed, check whether any transition in its preset becomes enabled
            // (this must be done to enable transitions which were disabled because their firing would have violated
            // the place's bound)
            const Place::IncomingEdgeList& placeIncoming = place->incomingArcs();
            for (unsigned int j = 0; j < placeIncoming.size(); j++) {
                TransitionToPlace* placeIncomingArc = placeIncoming[j];
                Transition* presetTransition = placeIncomingArc->source();
                
                if (oldTimeMarking[*presetTransition] == TimeMarking::Disabled && // it was previously disabled
                    marking + placeIncomingArc->multiplicity() > place->bound() && // maybe because its firing would have violated place's bounds
                    checkEnabled(*presetTransition, newPlaceMarking)) { // but now it is enabled
                        newTimeMarking[*presetTransition] = 0; // enable it
                }
            }
             */
        }
        
        for (unsigned int i = 0; i < outgoing.size(); i++) {
            TransitionToPlace* outgoingArc = outgoing[i];
            const Place* place = outgoingArc->target();
            const unsigned int addedTokens = outgoingArc->multiplicity();
            const unsigned int previousMarking = newPlaceMarking[*place];
            const unsigned int newMarking = previousMarking + addedTokens;
            newPlaceMarking[*place] = newMarking;

            // update the time marking for all transitions that have become active
            const Place::OutgoingEdgeList& placeOutgoing = place->outgoingEdges();
            for (unsigned j = 0; j < placeOutgoing.size(); j++) {
                PlaceToTransition* outgoingArc = placeOutgoing[j];
                const Transition* candidate = static_cast<const Transition*>(outgoingArc->target());
                const unsigned int multiplicity = outgoingArc->multiplicity();
                
                // if the transition was previously disabled and has become enabled due to the firing, set its time
                // marking to 0
                if (newTimeMarking[*candidate] == TimeMarking::Disabled && // it was previously disabled
                    previousMarking < multiplicity && // the place did not contain enough tokens before the firing
                    newMarking >= multiplicity && // but now it does
                    checkEnabled(*candidate, newPlaceMarking)) { // and thereby it really becomes enabled
                    newTimeMarking[*candidate] = 0;
                }
            }
            
            /* don't check for bound violations
            // for each place to which tokens are added, check whether any transition in its preset becomes disabled
            // because its firing would then violate the place's bounds
            const Place::IncomingEdgeList& placeIncoming = place->incomingArcs();
            for (unsigned j = 0; j < placeIncoming.size(); j++) {
                TransitionToPlace* placeIncomingArc = placeIncoming[j];
                Transition* presetTransition = placeIncomingArc->source();

                if (oldTimeMarking[*presetTransition] != TimeMarking::Disabled && // it was previously enabled
                    newMarking + placeIncomingArc->multiplicity() > place->bound()) { // it would add too many tokens to the place
                    newTimeMarking[*presetTransition] = TimeMarking::Disabled;
                }
            }
             */
        }
        
        newTimeMarking[transition] = remainEnabled ? 0 : TimeMarking::Disabled;
        
        return NetState(newPlaceMarking, newTimeMarking);
    }
}