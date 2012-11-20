//
//  RemoveDeadlocksFromMPP.h
//  Tippi
//
//  Created by Kristian Duske on 19.11.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__RemoveDeadlocksFromMPP__
#define __Tippi__RemoveDeadlocksFromMPP__

#include <iostream>

namespace Tippi {
    class MPP;
    
    class RemoveDeadlocksFromMPP {
    private:
        MPP& m_mpp;
    public:
        RemoveDeadlocksFromMPP(MPP& mpp);
        void operator()();
    };
}

#endif /* defined(__Tippi__RemoveDeadlocksFromMPP__) */
