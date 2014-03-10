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

#include "ConstructMaximalNet.h"

#include "Net/IntervalNet.h"

namespace Tippi {
    struct LabelingFunction {
        String operator()(const Interval::Transition* transition) const {
            const Interval::Place::List presetIOPlaces = getPresetIOPlaces(transition);
            const Interval::Place::List postsetIOPlaces = getPostsetIOPlaces(transition);
            
            if (presetIOPlaces.size() == 1 && postsetIOPlaces.empty())
                return presetIOPlaces[0]->getName() + "?";
            if (presetIOPlaces.empty() && postsetIOPlaces.size() == 1)
                return postsetIOPlaces[0]->getName() + "!";
            return "";
        }
        
        Interval::Place::List getPresetIOPlaces(const Interval::Transition* transition) const {
            Interval::Place::List result;
            
            const Interval::PlaceToTransition::List& incoming = transition->getIncoming();
            Interval::PlaceToTransition::List::const_iterator it, end;
            for (it = incoming.begin(), end = incoming.end(); it != end; ++it) {
                Interval::PlaceToTransition* arc = *it;
                Interval::Place* place = arc->getSource();
                if (place->isInputPlace() || place->isOutputPlace())
                    result.push_back(place);
            }
            return result;
        }
        
        Interval::Place::List getPostsetIOPlaces(const Interval::Transition* transition) const {
            Interval::Place::List result;
            
            const Interval::TransitionToPlace::List& outgoing = transition->getOutgoing();
            Interval::TransitionToPlace::List::const_iterator it, end;
            for (it = outgoing.begin(), end = outgoing.end(); it != end; ++it) {
                Interval::TransitionToPlace* arc = *it;
                Interval::Place* place = arc->getTarget();
                if (place->isInputPlace() || place->isOutputPlace())
                    result.push_back(place);
            }
            return result;
        }
    };
    
    ConstructMaximalNet::NetPtr ConstructMaximalNet::operator()(NetPtr net) const {
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
        net->setTransitionLabels(LabelingFunction());
        return net;
    }
}
