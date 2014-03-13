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

#ifndef __Tippi__SimpleAutomaton__
#define __Tippi__SimpleAutomaton__

#include "StringUtils.h"
#include "Automaton.h"

#include <vector>

namespace Tippi {
    class SimpleAutomatonState;
    class SimpleAutomatonEdge : public AutomatonEdge<SimpleAutomatonState> {
    public:
        typedef std::vector<SimpleAutomatonEdge*> List;
    public:
        SimpleAutomatonEdge(SimpleAutomatonState* source, SimpleAutomatonState* target, const String& label, bool tau);
    };
    
    class SimpleAutomatonState : public AutomatonState<SimpleAutomatonEdge> {
    public:
        typedef String Key;
        struct KeyCmp {
            int operator() (const Key& lhs, const Key& rhs) const;
        };
    private:
        const String m_name;
    public:
        SimpleAutomatonState(const String& name);
        static const Key& getKey(const SimpleAutomatonState* state);
        
        const String& getName() const;
    };
    
    class SimpleAutomaton : public Automaton<SimpleAutomatonState, SimpleAutomatonEdge> {};
}

#endif /* defined(__Tippi__SimpleAutomaton__) */
