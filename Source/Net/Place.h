//
//  Place.h
//  TPNA
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef TPNA_Place_h
#define TPNA_Place_h

#include "GraphNode.h"
#include "NetNode.h"
#include "Arc.h"

namespace TPNA {
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
        m_bound(Unbounded),
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
