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

#include "RenderIntervalNet.h"

#include "CollectionUtils.h"
#include "GraphAlgorithms.h"
#include "GraphNode.h"
#include "GraphEdge.h"
#include "IntervalNet.h"
#include "NetNode.h"

#include <map>

namespace Tippi {
    class IntervalNetVisitor {
    private:
        typedef std::map<const NetNode*, size_t> IdMap;
        
        std::ostream& m_stream;
        size_t m_nodeId;
        IdMap m_nodeIdMap;
    public:
        IntervalNetVisitor(std::ostream& stream) :
        m_stream(stream),
        m_nodeId(1) {}
        
        void visitNode(const Interval::Place* place) {
            m_stream << getNodeId(place) << " [";
            printAttribute("label", place->getName());
            m_stream << ",";
            printAttribute("shape", "circle");
            m_stream << "];" << std::endl;
        }
        
        void visitNode(const Interval::Transition* transition) {
            m_stream << getNodeId(transition) << " [";
            printAttribute("label", transition->getName());
            m_stream << ",";
            printAttribute("xlabel", transition->getInterval().asString());
            m_stream << ",";
            printAttribute("shape", "square");
            m_stream << "];" << std::endl;
        }
        
        void visitEdge(const Interval::TransitionToPlace* arc) {
            const Interval::Transition* source = arc->getSource();
            const Interval::Place* target = arc->getTarget();
            
            m_stream << getNodeId(source) << " -> " << getNodeId(target) << ";" << std::endl;
        }
        
        void visitEdge(const Interval::PlaceToTransition* arc) {
            const Interval::Place* source = arc->getSource();
            const Interval::Transition* target = arc->getTarget();
            
            m_stream << getNodeId(source) << " -> " << getNodeId(target) << ";" << std::endl;
        }
    private:
        size_t getNodeId(const NetNode* node) {
            std::pair<IdMap::iterator, bool> insertPos = MapUtils::findInsertPos(m_nodeIdMap, node);
            if (insertPos.second)
                return insertPos.first->second;
            const size_t nodeId = m_nodeId++;
            m_nodeIdMap.insert(insertPos.first, std::make_pair(node, nodeId));
            return nodeId;
        }
        
        void printAttribute(const String& name, const String& value) {
            m_stream << name << "=\"" << value << "\"";
        }
    };
    
    void RenderIntervalNet::operator()(const NetPtr net, std::ostream& stream) {
        stream << "digraph {" << std::endl;

        IntervalNetVisitor visitor(stream);
        
        const Interval::Place::List& places = net->getPlaces();
        const Interval::Transition::List& transitions = net->getTransitions();
        Interval::Place::resetVisited(places.begin(), places.end());
        Interval::Transition::resetVisited(transitions.begin(), transitions.end());

        BreadthFirst<IntervalNetVisitor, IntervalNetVisitor> visitPlaces(visitor, visitor);
        BreadthFirst<IntervalNetVisitor, IntervalNetVisitor> visitTransitions(visitor, visitor);
        
        Interval::Place::List::const_iterator pIt, pEnd;
        for (pIt = places.begin(), pEnd = places.end(); pIt != pEnd; ++pIt) {
            const Interval::Place* place = *pIt;
            visitPlaces(place);
        }
        
        Interval::Transition::List::const_iterator tIt, tEnd;
        for (tIt = transitions.begin(), tEnd = transitions.end(); tIt != tEnd; ++tIt) {
            const Interval::Transition* transition = *tIt;
            visitTransitions(transition);
        }
        
        stream << "}" << std::endl;
    }

    String RenderIntervalNet::operator()(const NetPtr net) {
        StringStream str;
        (*this)(net, str);
        return str.str();
    }
}
