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

#ifndef Tippi_GraphEdge_h
#define Tippi_GraphEdge_h

#include <cassert>
#include <vector>

namespace Tippi {
    template <typename SourceT, typename TargetT>
    class GraphEdge {
    public:
        typedef std::vector<GraphEdge*> List;
        typedef SourceT Source;
        typedef TargetT Target;
    protected:
        SourceT* m_source;
        TargetT* m_target;
        size_t m_multiplicity;
        mutable bool m_visited;
    public:
        GraphEdge(SourceT* source, TargetT* target, const size_t multiplicity = 1) :
        m_source(source),
        m_target(target),
        m_multiplicity(multiplicity),
        m_visited(false) {
            assert(m_source != NULL);
            assert(m_target != NULL);
            assert(m_multiplicity > 0);
        }
        
        virtual ~GraphEdge() {
            m_source = NULL;
            m_target = NULL;
        }
        
        const SourceT* getSource() const {
            return m_source;
        }
        
        SourceT* getSource() {
            return m_source;
        }
        
        const TargetT* getTarget() const {
            return m_target;
        }
        
        TargetT* getTarget() {
            return m_target;
        }
        
        void removeFromSource() {
            assert(m_source != NULL);
            m_source->removeOutgoing(static_cast<typename SourceT::Outgoing*>(this));
            m_source = NULL;
        }
        
        void replaceSource(SourceT* newSource) {
            assert(m_source != NULL);
            assert(newSource != NULL);
            m_source->removeOutgoing(static_cast<typename SourceT::Outgoing*>(this));
            m_source = newSource;
            m_source->addOutgoing(static_cast<typename SourceT::Outgoing*>(this));
        }
        
        void removeFromTarget() {
            assert(m_target != NULL);
            m_target->removeIncoming(static_cast<typename TargetT::Incoming*>(this));
            m_target = NULL;
        }
        
        void replaceTarget(TargetT* newTarget) {
            assert(m_target != NULL);
            assert(newTarget != NULL);
            m_target->removeIncoming(static_cast<typename TargetT::Incoming*>(this));
            m_target = newTarget;
            m_target->addIncoming(static_cast<typename TargetT::Incoming*>(this));
        }
        
        size_t getMultiplicity() const {
            return m_multiplicity;
        }

        bool isLoop() const {
            return m_target == m_source;
        }
        
        bool isVisited() const {
            return m_visited;
        }
        
        void setVisited(const bool visited) const {
            m_visited = visited;
        }
    };
}

#endif
