//
//  GraphEdge.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

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
    public:
        GraphEdge(SourceT* source, TargetT* target, const size_t multiplicity = 1) :
        m_source(source),
        m_target(target),
        m_multiplicity(multiplicity) {
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
            m_source->removeOutgoing(this);
            m_source = NULL;
        }
        
        void removeFromTarget() {
            m_target->removeIncoming(this);
            m_target = NULL;
        }
        
        size_t getMultiplicity() const {
            return m_multiplicity;
        }
    };
}

#endif
