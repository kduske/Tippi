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

#include "RenderBehavior.h"

#include "CollectionUtils.h"
#include "Behavior.h"
#include "Graph/GraphAlgorithms.h"

namespace Tippi {
    class Visitor {
    private:
        typedef std::map<const Behavior::State*, size_t> IdMap;
        
        std::ostream& m_stream;
        size_t m_stateId;
        IdMap m_stateIdMap;
    public:
        Visitor(std::ostream& stream) :
        m_stream(stream),
        m_stateId(1) {}
        
        void operator()(const Behavior::State* state) {
            m_stream << getStateId(state) << " [";
            printAttribute("label", state->getNetState().asString("\n"));
            m_stream << ",";
            printAttribute("shape", "ellipse");
            if (state->isFinal()) {
                m_stream << ",";
                printAttribute("peripheries", "2");
            }
            m_stream << "];" << std::endl;
        }
        
        void operator()(const Behavior::Edge* edge) {
            const Behavior::State* source = edge->getSource();
            const Behavior::State* target = edge->getTarget();
            
            m_stream << getStateId(source) << " -> " << getStateId(target) << "[";
            printAttribute("label", edge->getLabel());
            m_stream << "];" << std::endl;
        }
    private:
        size_t getStateId(const Behavior::State* state) {
            std::pair<IdMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_stateIdMap, state);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t stateId = m_stateId++;
            m_stateIdMap.insert(insertPos.first, std::make_pair(state, stateId));
            return stateId;
        }
        
        void printAttribute(const String& name, const String& value) {
            m_stream << name << "=\"" << value << "\"";
        }
    };
    
    void RenderBehavior::operator()(const BehPtr behavior, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        Visitor visitor(stream);
        const Behavior::State::List& states = behavior->getStates();
        Behavior::State::resetVisited(states);
        
        Behavior::State::List::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const Behavior::State* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
}