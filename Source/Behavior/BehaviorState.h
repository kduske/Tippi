//
//  BehaviorState.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__BehaviorState__
#define __Tippi__BehaviorState__

#include "GraphNode.h"
#include "BehaviorEdge.h"
#include "NetState.h"

#include <map>
#include <vector>

namespace Tippi {
    
    class BehaviorState : public GraphNode<BehaviorEdge, BehaviorEdge> {
    private:
        NetState m_netState;
        bool m_final;
    public:
        BehaviorState(const NetState& netState, bool final);
        ~BehaviorState();
        
        inline const NetState& netState() const {
            return m_netState;
        }
        
        inline bool isFinal() const {
            return m_final;
        }
    };
    
    typedef std::vector<BehaviorState*> BehaviorStateList;
    typedef std::map<NetState, BehaviorState*, StateComparator> BehaviorStateMap;
    typedef std::pair<BehaviorStateMap::iterator, bool> BehaviorStateMapInsertResult;
    typedef std::pair<NetState, BehaviorState*> BehaviorStateMapEntry;
}

#endif /* defined(__Tippi__BehaviorState__) */
