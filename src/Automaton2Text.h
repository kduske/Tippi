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
        template <class State, class Edge>
        void operator()(const Automaton<State,Edge>* automaton, std::ostream& stream) const {
            stream << "AUTOMATON" << std::endl;
            writeStates(automaton, stream);
            writeEdges(automaton, stream);
            writeInitialState(automaton, stream);
            writeFinalStates(automaton, stream);
        }
    private:
        template <class State, class Edge>
        void writeStates(const Automaton<State,Edge>* automaton, std::ostream& stream) const {
            writeStates<State>(automaton->getStates(), stream, "STATES");
        }
        
        template <class State, class Edge>
        void writeEdges(const Automaton<State,Edge>* automaton, std::ostream& stream) const {
            const typename Edge::Set& edges = automaton->getEdges();
            
            typename Edge::Set::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                const Edge* edge = *it;
                stream << "TRANSITION " << edge->getLabel() << "; FROM " << edge->getSource()->getName() << "; TO " << edge->getTarget()->getName() << ";" << std::endl;
            }
        }

        template <class State, class Edge>
        void writeInitialState(const Automaton<State,Edge>* automaton, std::ostream& stream) const {
            const State* initialState = automaton->getInitialState();
            if (initialState != NULL)
                stream << "INITIALSTATE " << initialState->getName() << ";" << std::endl;
        }

        template <class State, class Edge>
        void writeFinalStates(const Automaton<State,Edge>* automaton, std::ostream& stream) const {
            writeStates<State>(automaton->getFinalStates(), stream, "FINALSTATES");
        }
        
        
        template <class State>
        void writeStates(const typename State::Set& states, std::ostream& stream, const String& header) const {
            if (!states.empty()) {
                stream << header << " ";
                
                typename State::Set::const_iterator it = states.begin();
                const typename State::Set::const_iterator end = states.end();
                while (it != end) {
                    const State* state = *it;
                    stream << state->getName();
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
