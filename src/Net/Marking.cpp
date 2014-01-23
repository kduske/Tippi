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
    m_marking(count, 0) {}
    
    Marking Marking::createMarking(const size_t m1) {
        Marking marking(1);
        size_t i = 0;
        marking[i++] = m1;
        return marking;
    }
    
    Marking Marking::createMarking(const size_t m1, const size_t m2) {
        Marking marking(2);
        size_t i = 0;
        marking[i++] = m1;
        marking[i++] = m2;
        return marking;
    }
    
    Marking Marking::createMarking(const size_t m1, const size_t m2, const size_t m3) {
        Marking marking(3);
        size_t i = 0;
        marking[i++] = m1;
        marking[i++] = m2;
        marking[i++] = m3;
        return marking;
    }
    
    Marking Marking::createMarking(const size_t m1, const size_t m2, const size_t m3, const size_t m4) {
        Marking marking(4);
        size_t i = 0;
        marking[i++] = m1;
        marking[i++] = m2;
        marking[i++] = m3;
        marking[i++] = m4;
        return marking;
    }
    
    Marking Marking::createMarking(const size_t m1, const size_t m2, const size_t m3, const size_t m4, const size_t m5) {
        Marking marking(5);
        size_t i = 0;
        marking[i++] = m1;
        marking[i++] = m2;
        marking[i++] = m3;
        marking[i++] = m4;
        marking[i++] = m5;
        return marking;
    }
    
    Marking Marking::createMarking(const size_t m1, const size_t m2, const size_t m3, const size_t m4, const size_t m5, const size_t m6) {
        Marking marking(6);
        size_t i = 0;
        marking[i++] = m1;
        marking[i++] = m2;
        marking[i++] = m3;
        marking[i++] = m4;
        marking[i++] = m5;
        marking[i++] = m6;
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

    struct NullTranslator {
        void operator()(std::ostream& str, const size_t marking) const {
            str << marking;
        }
    };

    String Marking::asString() const {
        return asString(NullTranslator());
    }
}
