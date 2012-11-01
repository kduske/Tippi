//
//  Arc.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_Arc_h
#define Tippi_Arc_h

#include "GraphEdge.h"

namespace Tippi {
    class Place;
    class Transition;
    
    typedef GraphEdge<Transition, Place> TransitionToPlace;
    typedef GraphEdge<Place, Transition> PlaceToTransition;
}

#endif
