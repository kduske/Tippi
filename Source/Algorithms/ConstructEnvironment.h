//
//  ConstructEnvironment.h
//  Tippi
//
//  Created by Kristian Duske on 01.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__ConstructEnvironment__
#define __Tippi__ConstructEnvironment__

namespace Tippi {
    class Net;
    
    class ConstructEnvironment {
    private:
        Net& m_net;
    public:
        ConstructEnvironment(Net& net);
        void operator()();
    };
}

#endif /* defined(__Tippi__ConstructEnvironment__) */
