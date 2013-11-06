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

#include "ConstructMaximalNetFilter.h"

#include "Net/IntervalNet.h"

namespace Tippi {
    ConstructMaximalNetFilter::NetPtr ConstructMaximalNetFilter::operator()(NetPtr net) const {
        const Interval::Place::List& places = net->getPlaces();
        Interval::Place::List::const_iterator it, end;
        for (it = places.begin(), end = places.end(); it != end; ++it) {
            Interval::Place* place = *it;
            if (place->isInputPlace()) {
                assert(!place->isOutputPlace());
                Interval::Transition* transition = net->createTransition("t" + place->getName(), Interval::TimeInterval());
                net->connect(transition, place);
            } else if (place->isOutputPlace()) {
                assert(!place->isInputPlace());
                Interval::Transition* transition = net->createTransition("t" + place->getName(), Interval::TimeInterval());
                net->connect(place, transition);
            }
        }
        return net;
    }
}
