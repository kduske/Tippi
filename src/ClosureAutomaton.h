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

#ifndef __Tippi__ClosureAutomaton__
#define __Tippi__ClosureAutomaton__

#include "StringUtils.h"
#include "Graph/GraphEdge.h"
#include "Graph/GraphNode.h"
#include "Net/IntervalNetState.h"

#include <vector>

namespace Tippi {
    namespace Closure {
        class State;
        
        class Edge : public GraphEdge<State, State> {
        public:
            typedef std::vector<Edge*> List;
        private:
            String m_label;
        public:
            Edge(State* source, State* target, const String& label);
            
            bool operator<(const Edge& rhs) const;
            bool operator<(const Edge* rhs) const;
            int compare(const Edge& rhs) const;
            
            const String& getLabel() const;
        };
    }
}
#endif /* defined(__Tippi__ClosureAutomaton__) */
