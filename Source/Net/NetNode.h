//
//  NetNode.h
//  Tippi
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_NetNode_h
#define Tippi_NetNode_h

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace Tippi {
    class NetNode {
    public:
        static const size_t InvalidIndex;
    protected:
        size_t m_index;
        std::string m_name;
    public:
        NetNode(const std::string& name, size_t index = InvalidIndex) :
        m_name(name),
        m_index(index) {}
        
        inline const std::string& name() const {
            return m_name;
        }
        
        inline const size_t index() const {
            return m_index;
        }
        
        inline void setIndex(size_t index) {
            assert(m_index == InvalidIndex);
            assert(index != InvalidIndex);
            m_index = index;
        }
    };
}

#endif
