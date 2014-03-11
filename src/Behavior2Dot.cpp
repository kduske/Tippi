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

#include "Behavior2Dot.h"

#include "CollectionUtils.h"
#include "GraphAlgorithms.h"

namespace Tippi {
    class BehaviorVisitor {
    private:
        typedef std::map<const BehaviorState*, size_t> IdMap;
        
        std::ostream& m_stream;
        size_t m_stateId;
        IdMap m_stateIdMap;
    public:
        BehaviorVisitor(std::ostream& stream) :
        m_stream(stream),
        m_stateId(1) {}
        
        void operator()(const BehaviorState* state) {
            m_stream << getStateId(state) << " [";
            printAttribute("label", state->asString("\n"));
            m_stream << ",";
            printAttribute("shape", "ellipse");
            if (state->isFinal()) {
                m_stream << ",";
                printAttribute("peripheries", "2");
            }
            m_stream << "];" << std::endl;
        }
        
        void operator()(const BehaviorEdge* edge) {
            const BehaviorState* source = edge->getSource();
            const BehaviorState* target = edge->getTarget();
            
            m_stream << getStateId(source) << " -> " << getStateId(target) << " [";
            printAttribute("label", edge->getLabel());
            m_stream << "];" << std::endl;
        }
    private:
        size_t getStateId(const BehaviorState* state) {
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
    
    void Behavior2Dot::operator()(const Behavior::Ptr behavior, std::ostream& stream) {
        stream << "digraph {" << std::endl;
        
        BehaviorVisitor visitor(stream);
        const BehaviorState::Set& states = behavior->getStates();
        BehaviorState::resetVisited(states.begin(), states.end());
        
        BehaviorState::Set::const_iterator it, end;
        for (it = states.begin(), end = states.end(); it != end; ++it) {
            const BehaviorState* state = *it;
            visitNode(state, visitor, visitor);
        }
        
        stream << "}" << std::endl;
    }
}
