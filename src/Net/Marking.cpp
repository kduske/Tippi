//
//  Marking.cpp
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#include "Marking.h"

#include "Net/NetNode.h"

#include <cassert>

namespace Tippi {
    Marking::Marking(const size_t placeCount) :
    m_marking(placeCount) {}
    
    const size_t& Marking::operator[](const NetNode* node) const {
        assert(node != NULL);
        assert(node->getIndex() < m_marking.size());
        return m_marking[node->getIndex()];
    }
    
    size_t& Marking::operator[](const NetNode* node) {
        assert(node != NULL);
        assert(node->getIndex() < m_marking.size());
        return m_marking[node->getIndex()];
    }
}
