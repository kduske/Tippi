//
//  Place.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_Place_h
#define Tippi_Place_h

#include "GraphNode.h"
#include "NetNode.h"

#include "Transition.h"
#include "Arc.h"

namespace Tippi {
    class Place : public GraphNode<TransitionToPlace, PlaceToTransition>, public NetNode {
    public:
        static const unsigned int Unbounded = 0;
    protected:
        unsigned int m_bound;
        bool m_inputPlace;
        bool m_outputPlace;
    public:
        Place(const std::string& name, unsigned int bound = Unbounded) :
        GraphNode(),
        NetNode(name),
        m_bound(bound),
        m_inputPlace(false),
        m_outputPlace(false) {}

        inline unsigned int bound() const {
            return m_bound;
        }
        
        inline bool inputPlace() const {
            return m_inputPlace;
        }
        
        inline void setInputPlace(bool inputPlace) {
            m_inputPlace = inputPlace;
        }
        
        inline bool outputPlace() const {
            return m_outputPlace;
        }
        
        inline void setOutputPlace(bool outputPlace) {
            m_outputPlace = outputPlace;
        }
    };
}

#endif
