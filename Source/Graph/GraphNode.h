//
//  GraphNode.h
//  TPNA
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef TPNA_GraphNode_h
#define TPNA_GraphNode_h

#include <algorithm>
#include <vector>

namespace TPNA {
    template <typename IncomingEdge, typename OutgoingEdge>
    class GraphNode {
    public:
        typedef std::vector<IncomingEdge*> IncomingEdgeList;
        typedef std::vector<OutgoingEdge*> OutgoingEdgeList;
    protected:
        IncomingEdgeList m_incomingEdges;
        OutgoingEdgeList m_outgoingEdges;
    public:
        GraphNode() {}
        virtual ~GraphNode() {}
    
        inline void addIncomingEdge(const IncomingEdge* edge) {
            m_incomingEdges.push_back(edge);
        }

        inline void removeIncomingEdge(const IncomingEdge* edge) {
            m_incomingEdges.erase(std::remove(m_incomingEdges.begin(), m_incomingEdges.end(), edge), m_incomingEdges.end());
        }

        inline const IncomingEdgeList& incomingEdges() const {
            return m_incomingEdges;
        }

        inline void addOutgoingEdge(const OutgoingEdge* edge) {
            m_outgoingEdges.push_back(edge);
        }

        inline void removeOutgoingEdge(const OutgoingEdge* edge) {
            m_outgoingEdges.erase(std::remove(m_outgoingEdges.begin(), m_outgoingEdges.end(), edge), m_outgoingEdges.end());
        }

        inline const OutgoingEdgeList& outgoingEdges() const {
            return m_outgoingEdges;
        }
    };
}

#endif
