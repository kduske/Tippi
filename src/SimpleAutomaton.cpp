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

#include "SimpleAutomaton.h"

namespace Tippi {
    SimpleAutomatonEdge::SimpleAutomatonEdge(SimpleAutomatonState* source, SimpleAutomatonState* target, const String& label, bool tau) :
    AutomatonEdge(source, target, label, tau) {}

    bool SimpleAutomatonEdge::operator<(const SimpleAutomatonEdge& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool SimpleAutomatonEdge::operator<(const SimpleAutomatonEdge* rhs) const {
        return compare(*rhs) < 0;
    }

    SimpleAutomatonState::SimpleAutomatonState(const String& name) :
    AutomatonState(name) {}

    bool SimpleAutomatonState::operator<(const SimpleAutomatonState& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool SimpleAutomatonState::operator<(const SimpleAutomatonState* rhs) const {
        return compare(*rhs) < 0;
    }

    SimpleAutomatonState* SimpleAutomaton::createState(const String& name) {
        SimpleAutomatonState* state = new SimpleAutomatonState(name);
        try {
            addState(state);
            return state;
        } catch (...) {
            delete state;
            throw;
        }
    }
}
