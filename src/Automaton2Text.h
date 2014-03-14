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

#ifndef __Tippi__Automaton2Text__
#define __Tippi__Automaton2Text__

#include "Automaton.h"
#include <iostream>

namespace Tippi {
    struct Automaton2Text {
        template <class A>
        void operator()(const A* automaton, std::ostream& stream) const {
            stream << "AUTOMATON" << std::endl;
            writeStates(automaton, stream);
            writeEdges(automaton, stream);
            writeInitialState(automaton, stream);
            writeFinalStates(automaton, stream);
        }
    private:
        template <class A>
        void writeStates(const A* automaton, std::ostream& stream) const {
            writeStates<typename A::State>(automaton->getStates(), stream, "STATES");
        }
        
        template <class A>
        void writeEdges(const A* automaton, std::ostream& stream) const {
            const typename A::EdgeSet& edges = automaton->getEdges();
            
            typename A::EdgeSet::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const typename A::Edge* edge = *it;
                stream << "TRANSITION " << edge->getLabel() << "; FROM " << edge->getSource()->getId() << "; TO " << edge->getTarget()->getId() << ";" << std::endl;
            }
        }

        template <class A>
        void writeInitialState(const A* automaton, std::ostream& stream) const {
            const typename A::State* initialState = automaton->getInitialState();
            if (initialState != NULL)
                stream << "INITIALSTATE " << initialState->getId() << ";" << std::endl;
        }

        template <class A>
        void writeFinalStates(const A* automaton, std::ostream& stream) const {
            writeStates<typename A::State>(automaton->getFinalStates(), stream, "FINALSTATES");
        }
        
        
        template <class State, class C>
        void writeStates(const C& states, std::ostream& stream, const String& header) const {
            if (!states.empty()) {
                stream << header << " ";
                
                typename C::const_iterator it = states.begin();
                const typename C::const_iterator end = states.end();
                while (it != end) {
                    const State* state = *it;
                    stream << state->getId();
                    ++it;
                    if (it != end)
                        stream << ",";
                }
                stream << ";" << std::endl;
            }
        }
    };
}

#endif /* defined(__Tippi__Automaton2Text__) */
