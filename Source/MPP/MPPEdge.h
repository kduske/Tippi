//
//  MPPEdge.h
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__MPPEdge__
#define __Tippi__MPPEdge__

#include "GraphEdge.h"

#include <vector>

namespace Tippi {
    class MPPState;
    class Transition;
    
    class MPPEdge : public GraphEdge<MPPState, MPPState> {
    private:
        unsigned int m_minTime;
        unsigned int m_maxTime;
        const Transition& m_transition;
    public:
        MPPEdge(MPPState* source, MPPState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition);

        inline unsigned int minTime() const {
            return m_minTime;
        }
        
        inline unsigned int maxTime() const {
            return m_maxTime;
        }
        
        inline const Transition& transition() const {
            return m_transition;
        }
    };

    typedef std::vector<MPPEdge*> MPPEdgeList;
}

#endif /* defined(__Tippi__MPPEdge__) */
