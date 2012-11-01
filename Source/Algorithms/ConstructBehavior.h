//
//  ConstructBehavior.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__ConstructBehavior__
#define __Tippi__ConstructBehavior__

namespace Tippi {
    class Behavior;
    class FiringRule;
    class Net;

    class ConstructBehavior {
    private:
        const Net& m_net;
        Behavior& m_behavior;
        FiringRule& m_firingRule;
    public:
        ConstructBehavior(const Net& net, Behavior& behavior, FiringRule& firingRule);
        void operator()();
    };
}

#endif /* defined(__Tippi__ConstructBehavior__) */
