//
//  TimedFiringRule.h
//  Tippi
//
//  Created by Kristian Duske on 17.09.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__TimedFiringRule__
#define __Tippi__TimedFiringRule__

#include "FiringRule.h"

#include "PlaceMarking.h"
#include "TimeMarking.h"
#include "NetTypes.h"

namespace Tippi {
    class Net;
    class NetState;
    class Transition;
    
    class FiringRule {
    private:
        Net& m_net;
        
        bool checkEnabled(const Transition& transition, const PlaceMarking& placeMarking) const;
    public:
        FiringRule(Net& net);
        
        TimeMarking initialTimeMarking() const;
        NetState initialState() const;

        bool isEnabled(const Transition& transition, const NetState& state) const;
        bool isFireable(const Transition& transition, const NetState& state) const;
        TransitionList fireableTransitions(const NetState& state) const;
        
        unsigned int minimumTime(const NetState& state) const;
        unsigned int maximumTime(const NetState& state) const;
        
        NetState passTime(const NetState& state, unsigned int time) const;
        NetState fire(const Transition& transition, const NetState& state) const;
    };
}
#endif /* defined(__Tippi__TimedFiringRule__) */
