//
//  PlaceMarking.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#ifndef __Tippi__PlaceMarking__
#define __Tippi__PlaceMarking__

#include <vector>

namespace Tippi {
    class NetNode;
    
    class Marking {
    private:
        std::vector<size_t> m_marking;
    public:
        Marking(const size_t placeCount);
        
        const size_t& operator[](const NetNode* node) const;
        size_t& operator[](const NetNode* node);
    };
}

#endif /* defined(__Tippi__PlaceMarking__) */
