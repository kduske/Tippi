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
        unsigned int m_time;
        const Transition& m_transition;
    public:
        BehaviorEdge(BehaviorState* source, BehaviorState* target, unsigned int time, const Transition& transition);
        
        inline unsigned int time() const {
            return m_time;
        }
        
        inline const Transition& transition() const {
            return m_transition;
        }
    };
    
    typedef std::vector<BehaviorEdge*> BehaviorEdgeList;
}

#endif /* defined(__Tippi__BehaviorEdge__) */
