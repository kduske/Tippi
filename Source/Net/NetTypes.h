//
//  NetTypes.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_NetTypes_h
#define Tippi_NetTypes_h

#include "Arc.h"

#include <set>
#include <vector>

namespace Tippi {
    class Place;
    class Transition;
    class PlaceMarking;
    
    typedef std::vector<Place*> PlaceList;
    typedef std::vector<Transition*> TransitionList;
    typedef std::vector<PlaceMarking> PlaceMarkingList;
    typedef std::set<PlaceMarking> PlaceMarkingSet;
    typedef std::vector<TransitionToPlace*> TransitionToPlaceArcList;
    typedef std::vector<PlaceToTransition*> PlaceToTransitionArcList;
}

#endif
