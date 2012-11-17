//
//  ConvertMPPToDot.h
//  Tippi
//
//  Created by Kristian Duske on 17.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__ConvertMPPToDot__
#define __Tippi__ConvertMPPToDot__

#include <iostream>
#include <map>
#include <string>

namespace Tippi {
    class MPP;
    class MPPEdge;
    class MPPState;
    
    class ConvertMPPToDot {
    private:
        const MPP& m_mpp;
        std::ostream& m_stream;
        std::string m_graphName;

        typedef std::map<const MPPState*, std::string> StateNames;
        typedef std::pair<const MPPState*, std::string> StateNameEntry;
        
        StateNames m_stateNames;
        
        std::string printStateName(const MPPState& state);
        void visitState(MPPState* state);
        void visitEdge(MPPEdge* edge);
    public:
        ConvertMPPToDot(const MPP& mpp, std::ostream& stream, const std::string& graphName);
        void operator()();
    };
}


#endif /* defined(__Tippi__ConvertMPPToDot__) */
