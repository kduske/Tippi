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

#ifndef Tippi_GraphNode_h
#define Tippi_GraphNode_h

#include "CollectionUtils.h"

#include <algorithm>
#include <set>

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
    
        void addIncoming(IncomingT* edge) {
            m_incoming.push_back(edge);
        }

        void removeIncoming(IncomingT* edge) {
            VectorUtils::remove(m_incoming, edge);
        }
        
        template <typename NodeT>
        void replaceAsSource(NodeT* newSource) {
            while (!m_outgoing.empty()) {
                OutgoingT* edge = m_outgoing.back();
                edge->replaceSource(newSource);
            }
        }
        
        template <typename NodeT>
        void replaceAsTarget(NodeT* newTarget) {
            while (!m_incoming.empty()) {
                IncomingT* edge = m_incoming.back();
                edge->replaceTarget(newTarget);
            }
        }
        
        const IncomingList& getIncoming() const {
            return m_incoming;
        }

        void addOutgoing(OutgoingT* edge) {
            m_outgoing.push_back(edge);
        }

        void removeOutgoing(OutgoingT* edge) {
            VectorUtils::remove(m_outgoing, edge);
        }

        const OutgoingList& getOutgoing() const {
            return m_outgoing;
        }
        
        bool isInPreset(const typename IncomingT::Source* node) const {
            typename IncomingList::const_iterator it, end;
            for (it = m_incoming.begin(), end = m_incoming.end(); it != end; ++it) {
                const IncomingT* edge = *it;
                if (edge->getSource() == node)
                    return true;
            }
            return false;
        }
        
        bool isInPostset(const typename OutgoingT::Target* node) const {
            typename OutgoingList::const_iterator it, end;
            for (it = m_outgoing.begin(), end = m_outgoing.end(); it != end; ++it) {
                const OutgoingT* edge = *it;
                if (edge->getTarget() == node)
                    return true;
            }
            return false;
        }
        
        template <typename Cmp>
        bool isPresetSubsetOf(const std::set<typename IncomingT::Source*, Cmp>& set) const {
            typename IncomingList::const_iterator it, end;
            for (it = m_incoming.begin(), end = m_incoming.end(); it != end; ++it) {
                IncomingT* edge = *it;
                typename IncomingT::Source* source = edge->getSource();
                if (!set.count(source) == 1)
                    return false;
            }
            return true;
        }
        
        template <typename Cmp>
        bool isPresetSubsetOfIgnoringLoops(const std::set<typename IncomingT::Source*, Cmp>& set) const {
            typename IncomingList::const_iterator it, end;
            for (it = m_incoming.begin(), end = m_incoming.end(); it != end; ++it) {
                IncomingT* edge = *it;
                typename IncomingT::Source* source = edge->getSource();
                if (this != source && set.count(source) == 0)
                    return false;
            }
            return true;
        }
        
        bool isVisited() const {
            return m_visited;
        }
        
        void setVisited(const bool visited) const {
            m_visited = visited;
        }
        
        template <typename I>
        static void resetVisited(I cur, I end) {
            while (cur != end) {
                (*cur)->setVisited(false);
                ++cur;
            }
        }
        
        static OutgoingT* connectToTarget(typename OutgoingT::Source* source, typename OutgoingT::Target* target) {
            OutgoingT* e = new OutgoingT(source, target);
            source->addOutgoing(e);
            target->addIncoming(e);
            return e;
        }
        
        static IncomingT* connectToSource(typename IncomingT::Target* target, typename IncomingT::Source* source) {
            return IncomingT::Source::connectToTarget(source, target);
        }
    };
}

#endif
