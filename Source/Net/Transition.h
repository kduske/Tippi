//
//  Transition.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_Transition_h
#define Tippi_Transition_h

#include "GraphNode.h"
#include "NetNode.h"

#include "Place.h"
#include "Arc.h"

#include <cassert>

namespace Tippi {
    class Transition : public GraphNode<PlaceToTransition, TransitionToPlace>, public NetNode {
    public:
        static const unsigned int Infinite;
    protected:
        unsigned int m_earliestFiringTime;
        unsigned int m_latestFiringTime;
        bool m_internal;
    public:
        Transition(const std::string& name, unsigned int earliestFiringTime = 0, unsigned int latestFiringTime = Infinite, bool internal = true) :
        GraphNode(),
        NetNode(name),
        m_earliestFiringTime(earliestFiringTime),
        m_latestFiringTime(latestFiringTime),
        m_internal(internal) {
            assert(m_earliestFiringTime <= m_latestFiringTime || m_latestFiringTime == Infinite);
        }
        
        inline unsigned int earliestFiringTime() const {
            return m_earliestFiringTime;
        }
        
        inline unsigned int latestFiringTime() const {
            return m_latestFiringTime;
        }
        
        inline bool internal() const {
            return m_internal;
        }
    };
}

#endif
