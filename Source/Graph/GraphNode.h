//
//  GraphNode.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_GraphNode_h
#define Tippi_GraphNode_h

#include <algorithm>
#include <vector>

namespace Tippi {
    template <typename IncomingEdge, typename OutgoingEdge>
    class GraphNode {
    public:
        typedef std::vector<IncomingEdge*> IncomingEdgeList;
        typedef std::vector<OutgoingEdge*> OutgoingEdgeList;
    private:
        mutable bool m_visited;
    protected:
        IncomingEdgeList m_incomingEdges;
        OutgoingEdgeList m_outgoingEdges;
    public:
        GraphNode() :
        m_visited(false) {}
        
        virtual ~GraphNode() {}
    
        inline void addIncomingEdge(IncomingEdge* edge) {
            m_incomingEdges.push_back(edge);
        }

        inline void removeIncomingEdge(IncomingEdge* edge) {
            m_incomingEdges.erase(std::remove(m_incomingEdges.begin(), m_incomingEdges.end(), edge), m_incomingEdges.end());
        }

        inline const IncomingEdgeList& incomingEdges() const {
            return m_incomingEdges;
        }

        inline void addOutgoingEdge(OutgoingEdge* edge) {
            m_outgoingEdges.push_back(edge);
        }

        inline void removeOutgoingEdge(OutgoingEdge* edge) {
            m_outgoingEdges.erase(std::remove(m_outgoingEdges.begin(), m_outgoingEdges.end(), edge), m_outgoingEdges.end());
        }

        inline const OutgoingEdgeList& outgoingEdges() const {
            return m_outgoingEdges;
        }
        
        inline bool visited() const {
            return m_visited;
        }
        
        inline void setVisited(bool visited) const {
            m_visited = visited;
        }
    };
}

#endif
