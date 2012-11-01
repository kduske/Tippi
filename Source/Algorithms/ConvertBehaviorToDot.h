//
//  ConvertBehaviorToDot.h
//  Tippi
//
//  Created by Kristian Duske on 26.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__ConvertBehaviorToDot__
#define __Tippi__ConvertBehaviorToDot__

#include <iostream>
#include <map>
#include <string>

namespace Tippi {
    class Behavior;
    class BehaviorState;
    class BehaviorEdge;
    class FiringRule;
    class Net;
    
    class ConvertBehaviorToDot {
    private:
        const Behavior& m_behavior;
        std::ostream& m_stream;
        std::string m_graphName;
        
        typedef std::map<BehaviorState*, std::string> StateNames;
        typedef std::pair<BehaviorState*, std::string> StateNameEntry;
        
        StateNames m_stateNames;
        
        std::string printStateName(BehaviorState* state);
        void visitState(BehaviorState* state);
        void visitEdge(BehaviorEdge* edge);
    public:
        ConvertBehaviorToDot(const Behavior& behavior, std::ostream& stream, const std::string& graphName);
        void operator()();
    };
}


#endif /* defined(__Tippi__ConvertBehaviorToDot__) */
