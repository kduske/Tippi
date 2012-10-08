//
//  GraphEdge.h
//  TPNA
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef TPNA_GraphEdge_h
#define TPNA_GraphEdge_h

#include <cassert>

namespace TPNA {
    template <typename Source, typename Target>
    class GraphEdge {
    protected:
        Source* m_source;
        Target* m_target;
        unsigned int m_multiplicity;
    public:
        GraphEdge(Source* source, Target* target, unsigned int multiplicity = 1) :
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
        
        inline const Source* source() const {
            return m_source;
        }
        
        inline const Target* target() const {
            return m_target;
        }
        
        inline unsigned int multiplicity() const {
            return m_multiplicity;
        }
    };
}

#endif
