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
#include <cstdarg>

namespace Tippi {
    Marking::Marking(const size_t count) :
    m_marking(count, 0) {
    }
    
    Marking Marking::createMarking(const size_t count, ...) {
        Marking marking(count);
        
        va_list args;
        va_start(args, count);
        for (size_t i = 0; i < count; ++i)
            marking[i] = va_arg(args, size_t);
        va_end(args);
        
        return marking;
    }

    bool Marking::operator<(const Marking& rhs) const {
        return compare(rhs) < 0;
    }
    
    bool Marking::operator==(const Marking& rhs) const {
        return compare(rhs) == 0;
    }

    int Marking::compare(const Marking& rhs) const {
        assert(m_marking.size() == rhs.m_marking.size());
        for (size_t i = 0; i < m_marking.size(); ++i) {
            if (m_marking[i] < rhs.m_marking[i])
                return -1;
            if (m_marking[i] > rhs.m_marking[i])
                return 1;
        }
        return 0;
    }

    const size_t& Marking::operator[](const NetNode* node) const {
        assert(node != NULL);
        return (*this)[node->getIndex()];
    }
    
    size_t& Marking::operator[](const NetNode* node) {
        assert(node != NULL);
        return (*this)[node->getIndex()];
    }

    const size_t& Marking::operator[](const size_t index) const {
        assert(index < m_marking.size());
        return m_marking[index];
    }
    
    size_t& Marking::operator[](const size_t index) {
        assert(index < m_marking.size());
        return m_marking[index];
    }
    
    size_t Marking::getSize() const {
        return m_marking.size();
    }

    String Marking::asString() const {
        StringStream str;
        str << '[';
        for (size_t i = 0; i < m_marking.size() - 1; ++i)
            str << m_marking[i] << ',';
        if (!m_marking.empty())
            str << m_marking.back();
        str << "]";
        return str.str();
    }
}
