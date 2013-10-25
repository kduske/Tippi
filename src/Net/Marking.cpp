/*
 Copyright (C) 2013 Kristian Duske
 
 This file is part of Tippi.
 
 Tippi is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Tippi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Tippi. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Marking.h"

#include "Net/NetNode.h"

#include <cassert>

namespace Tippi {
    Marking::Marking(const size_t placeCount) :
    m_marking(placeCount, 0) {
    }
    
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

    size_t Marking::getSize() const {
        return m_marking.size();
    }
}
