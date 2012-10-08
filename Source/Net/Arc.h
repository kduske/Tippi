//
//  Arc.h
//  TPNA
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef TPNA_Arc_h
#define TPNA_Arc_h

#include "GraphEdge.h"

namespace TPNA {
    class Place;
    class Transition;
    
    typedef GraphEdge<Transition, Place> TransitionToPlace;
    typedef GraphEdge<Place, Transition> PlaceToTransition;
}

#endif
