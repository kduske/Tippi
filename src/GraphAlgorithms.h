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
    template <class Node, class NodeVisitor, class EdgeVisitor>
    void visitNode(Node* node, NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) {
        if (node->isVisited())
            return;

        node->setVisited(true);
        nodeVisitor(node);
        
        typedef typename Node::Outgoing::List OutList;
        const OutList& edges = node->getOutgoing();
        typename OutList::const_iterator it, end;
        for (it = edges.begin(), end = edges.end(); it != end; ++it) {
            typename Node::Outgoing* edge = *it;
            visitEdge(edge, nodeVisitor, edgeVisitor);
        }
    }
    
    template <class Edge, class NodeVisitor, class EdgeVisitor>
    void visitEdge(Edge* edge, NodeVisitor& nodeVisitor, EdgeVisitor& edgeVisitor) {
        if (edge->isVisited())
            return;

        edge->setVisited(true);
        visitNode(edge->getTarget(), nodeVisitor, edgeVisitor);
        edgeVisitor(edge);
    }
}

#endif
