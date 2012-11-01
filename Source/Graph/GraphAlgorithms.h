//
//  GraphAlgorithms.h
//  Tippi
//
//  Created by Kristian Duske on 26.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_GraphAlgorithms_h
#define Tippi_GraphAlgorithms_h

#include "GraphEdge.h"
#include "GraphNode.h"

#include <set>

namespace Tippi {
    template <class Object, typename NodeType>
    class NodeFunctor {
    private:
        Object* m_object;
        void (Object::*m_func)(NodeType*);
    public:
        NodeFunctor(Object* object, void (Object::*func)(NodeType*)) :
        m_object(object),
        m_func(func) {}
        
        void operator()(NodeType* node) {
            (m_object->*m_func)(node);
        }
    };
    
    template <class Object, typename EdgeType>
    class EdgeFunctor {
    private:
        Object* m_object;
        void (Object::*m_func)(EdgeType*);
    public:
        EdgeFunctor(Object* object, void (Object::*func)(EdgeType*)) :
        m_object(object),
        m_func(func) {}
        
        void operator()(EdgeType* edge) {
            (m_object->*m_func)(edge);
        }
    };
    
    template <typename NodeType, typename EdgeType>
    inline void setUnvisited(NodeType* node, std::set<NodeType*>& visitedNodes) {
        if (visitedNodes.count(node) > 0)
            return;
        
        visitedNodes.insert(node);
        node->setVisited(false);
        
        const typename NodeType::OutgoingEdgeList& outgoing = node->outgoingEdges();
        typename NodeType::OutgoingEdgeList::const_iterator edgeIt, edgeEnd;
        for (edgeIt = outgoing.begin(), edgeEnd = outgoing.end(); edgeIt != edgeEnd; ++edgeIt) {
            EdgeType* edge = *edgeIt;
            NodeType* successor = edge->target();
            setUnvisited<NodeType, EdgeType>(successor, visitedNodes);
        }
    }
    
    template <typename NodeType, typename EdgeType, typename NodeFuncType, typename EdgeFuncType>
    inline void doDepthFirst(NodeType* node, NodeFuncType nodeFunc, EdgeFuncType edgeFunc) {
        if (node->visited())
            return;
        
        nodeFunc(node);
        node->setVisited(true);
        
        const typename NodeType::OutgoingEdgeList& outgoing = node->outgoingEdges();
        typename NodeType::OutgoingEdgeList::const_iterator edgeIt, edgeEnd;
        for (edgeIt = outgoing.begin(), edgeEnd = outgoing.end(); edgeIt != edgeEnd; ++edgeIt) {
            EdgeType* edge = *edgeIt;
            
            NodeType* successor = edge->target();
            doDepthFirst<NodeType, EdgeType, NodeFuncType, EdgeFuncType>(successor, nodeFunc, edgeFunc);

            edgeFunc(edge);
        }
    }
    
    template <typename NodeType, typename EdgeType, typename NodeFuncType, typename EdgeFuncType>
    inline void depthFirst(NodeType* node, NodeFuncType nodeFunc, EdgeFuncType edgeFunc) {
        std::set<NodeType*> visitedNodes;
        setUnvisited<NodeType, EdgeType>(node, visitedNodes);
        
        doDepthFirst<NodeType, EdgeType, NodeFuncType, EdgeFuncType>(node, nodeFunc, edgeFunc);
    }
}

#endif
