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

#ifndef Tippi_GraphNode_h
#define Tippi_GraphNode_h

#include "CollectionUtils.h"

#include <algorithm>
#include <vector>

namespace Tippi {
    template <typename IncomingT, typename OutgoingT>
    class GraphNode {
    public:
        typedef IncomingT Incoming;
        typedef OutgoingT Outgoing;
        typedef std::vector<IncomingT*> IncomingList;
        typedef std::vector<OutgoingT*> OutgoingList;
    private:
        mutable bool m_visited;
    protected:
        IncomingList m_incoming;
        OutgoingList m_outgoing;
    public:
        GraphNode() :
        m_visited(false) {}
        
        virtual ~GraphNode() {}
    
        void addIncoming(Incoming* edge) {
            m_incoming.push_back(edge);
        }

        void removeIncoming(Incoming* edge) {
            VectorUtils::remove(m_incoming, edge);
        }
        
        const IncomingList& getIncoming() const {
            return m_incoming;
        }

        void addOutgoing(Outgoing* edge) {
            m_outgoing.push_back(edge);
        }

        void removeOutgoing(Outgoing* edge) {
            VectorUtils::remove(m_outgoing, edge);
        }

        const OutgoingList& getOutgoing() const {
            return m_outgoing;
        }
        
        bool isVisited() const {
            return m_visited;
        }
        
        void setVisited(const bool visited) const {
            m_visited = visited;
        }
        
        static OutgoingT* connectToTarget(typename Outgoing::Source* source, typename Outgoing::Target* target) {
            Outgoing* e = new Outgoing(source, target);
            source->addOutgoing(e);
            target->addIncoming(e);
            return e;
        }
        
        static Incoming* connectToSource(typename Incoming::Target* target, typename Incoming::Source* source) {
            return Incoming::Source::connectToTarget(source, target);
        }
    };
}

#endif
