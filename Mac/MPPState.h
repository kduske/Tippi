//
//  MPPState.h
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__MPPState__
#define __Tippi__MPPState__

#include "BehaviorState.h"
#include "GraphNode.h"
#include "MPPEdge.h"
#include "NetState.h"

#include <map>
#include <vector>

namespace Tippi {
    class MPPState : public GraphNode<MPPEdge, MPPEdge> {
    public:
        typedef enum {
            Undetermined,
            Deadlock,
            Controllable,
            NoDeadlock
        } DeadlockClass;
    private:
        DeadlockClass m_deadlockClass;
        NetStateSet m_netStates;
        bool m_final;
        MPPEdgeList m_incomingEdges;
        MPPEdgeList m_outgoingEdges;
    public:
        MPPState(DeadlockClass deadlockClass, const NetState& netState, bool final);
        
        inline DeadlockClass deadlockClass() const {
            return m_deadlockClass;
        }
        
        inline const NetStateSet& netStates() const {
            return m_netStates;
        }
        
        inline bool isFinal() const {
            return m_final;
        }
        
        inline void addNetState(const NetState& netState, bool final) {
            if (m_netStates.insert(netState).second)
                m_final |= final;
        }
    };

    typedef std::vector<MPPState*> MPPStateList;
    typedef std::map<NetState, MPPState*, StateComparator> MPPStateMap;
    typedef std::pair<MPPStateMap::iterator, bool> MPPStateMapInsertResult;
    typedef std::pair<NetState, MPPState*> MPPStateMapEntry;
}

#endif /* defined(__Tippi__MPPState__) */
