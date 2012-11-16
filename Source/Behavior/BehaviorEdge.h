//
//  BehaviorEdge.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__BehaviorEdge__
#define __Tippi__BehaviorEdge__

#include "GraphEdge.h"

#include <vector>

namespace Tippi {
    class BehaviorState;
    class Transition;
    
    class BehaviorEdge : public GraphEdge<BehaviorState, BehaviorState> {
    private:
        unsigned int m_minTime;
        unsigned int m_maxTime;
        const Transition& m_transition;
    public:
        BehaviorEdge(BehaviorState* source, BehaviorState* target, unsigned int minTime, unsigned int maxTime, const Transition& transition);
        
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
    
    typedef std::vector<BehaviorEdge*> BehaviorEdgeList;
}

#endif /* defined(__Tippi__BehaviorEdge__) */
