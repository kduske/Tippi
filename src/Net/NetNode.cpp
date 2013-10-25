//
//  NetNode.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#include "NetNode.h"

namespace Tippi {
    NetNode::NetNode(const String& name, const size_t index) :
    m_name(name),
    m_index(index) {}
    
    const String& NetNode::getName() const {
        return m_name;
    }
    
    size_t NetNode::getIndex() const {
        return m_index;
    }
}
