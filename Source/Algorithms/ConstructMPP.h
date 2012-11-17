//
//  ConstructMPP.h
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__ConstructMPP__
#define __Tippi__ConstructMPP__

#include "MPPState.h"

#include <map>
#include <set>
#include <vector>

namespace Tippi {
    class Behavior;
    class BehaviorEdge;
    class BehaviorState;
    class MPP;
    class Net;
    class NetState;
    
    class ConstructMPP {
    private:
        class ClosureEdge {
        private:
            const BehaviorEdge* m_behaviorEdge;
            unsigned int m_minTime;
            unsigned int m_maxTime;
        public:
            ClosureEdge(const BehaviorEdge& externalEdge);
            ClosureEdge(const BehaviorEdge& internalEdge, const ClosureEdge& externalEdge);
            
            inline bool operator<(const ClosureEdge& other) const {
                if (m_behaviorEdge < other.m_behaviorEdge)
                    return true;
                if (m_behaviorEdge > other.m_behaviorEdge)
                    return false;
                if (m_minTime < other.m_minTime)
                    return true;
                if (m_minTime > other.m_minTime)
                    return false;
                return m_maxTime < other.m_maxTime;
            }
            
            inline const BehaviorEdge& behaviorEdge() const {
                return *m_behaviorEdge;
            }
            
            inline unsigned int minTime() const {
                return m_minTime;
            }
            
            inline unsigned int maxTime() const {
                return m_maxTime;
            }
        };

        class Closure {
        public:
            typedef std::set<const BehaviorState*> StateSet;
            typedef std::set<ClosureEdge> EdgeSet;
        private:
            StateSet m_states;
            EdgeSet m_edges; // prevent duplicate outgoing edges!!!
        public:
            Closure(const BehaviorState& initialState);
            
            inline const StateSet& states() const {
                return m_states;
            }
            
            inline const EdgeSet& edges() const {
                return m_edges;
            }
            
            void addClosure(const BehaviorEdge& viaInternalEdge, const Closure& closure);
            void addEdge(const BehaviorEdge& externalEdge);
        };

        typedef std::map<const BehaviorState*, Closure> ClosureMap;
        typedef std::pair<const BehaviorState*, Closure> ClosureMapEntry;
        typedef std::pair<ClosureMap::iterator, bool> ClosureMapInsertResult;
        typedef std::map<const BehaviorState*, MPPState::DeadlockClass> DeadlockClassMap;
                
        const Net& m_net;
        const Behavior& m_behavior;
        MPP& m_mpp;
        
        ClosureMap m_closures;
        DeadlockClassMap m_deadlockClasses;
        
        const Closure& buildClosure(const BehaviorState& behaviorState);
        MPPState::DeadlockClass determineDeadlockClass(const BehaviorState& behaviorState);
        MPPState* processState(const BehaviorState& behaviorState, bool initialState);
    public:
        ConstructMPP(const Net& net, const Behavior& behavior, MPP& mpp);
        void operator()();
    };
}

#endif /* defined(__Tippi__ConstructMPP__) */
