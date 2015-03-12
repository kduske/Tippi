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

#ifndef Tippi_GraphAlgorithms_h
#define Tippi_GraphAlgorithms_h

namespace Tippi {
    template <class NodeVisitor, class EdgeVisitor>
    class BreadthFirst {
    private:
        NodeVisitor& m_nodeVisitor;
        EdgeVisitor& m_edgeVisitor;
    public:
        BreadthFirst(NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) :
        m_nodeVisitor(nodeVisitor),
        m_edgeVisitor(edgeVisitor) {}
        
        template <typename Node>
        void operator()(Node* node) {
            visit(node);
        }
    private:
        template <typename Node>
        void visit(Node* node) {
            if (node->isVisited())
                return;
            
            node->setVisited(true);
            m_nodeVisitor.visitNode(node);

            typedef typename Node::Outgoing::List OutList;
            const OutList& edges = node->getOutgoing();
            typename OutList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                typename Node::Outgoing* edge = *it;
                
                edge->setVisited(true);
                m_edgeVisitor.visitEdge(edge);
                
                visit(edge->getTarget());
            }
        }
    };
    
    template <class Node, class NodeVisitor, class EdgeVisitor>
    void breadthFirst(Node* root, NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) {
        BreadthFirst<NodeVisitor, EdgeVisitor> visitNode(nodeVisitor, edgeVisitor);
        visitNode(root);
    }
    
    template <class NodeVisitor, class EdgeVisitor>
    class DepthFirst {
    private:
        NodeVisitor& m_nodeVisitor;
        EdgeVisitor& m_edgeVisitor;
    public:
        DepthFirst(NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) :
        m_nodeVisitor(nodeVisitor),
        m_edgeVisitor(edgeVisitor) {}
        
        template <typename Node>
        void operator()(Node* node) {
            visit(node);
        }
    private:
        template <typename Node>
        void visit(Node* node) {
            if (node->isVisited())
                return;
            
            node->setVisited(true);
            m_nodeVisitor.initNode(node);
            
            typedef typename Node::Outgoing::List OutList;
            const OutList& edges = node->getOutgoing();
            typename OutList::const_iterator it, end;
            for (it = edges.begin(), end = edges.end(); it != end; ++it) {
                typename Node::Outgoing* edge = *it;
                
                edge->setVisited(true);
                visit(edge->getTarget());
                
                m_edgeVisitor.visitEdge(edge);
            }

            m_nodeVisitor.visitNode(node);
        }
    };

    
    template <class Node, class NodeVisitor, class EdgeVisitor>
    void depthFirst(Node* root, NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) {
        DepthFirst<NodeVisitor, EdgeVisitor> visitNode(nodeVisitor, edgeVisitor);
        visitNode(root);
    }
}

#endif
