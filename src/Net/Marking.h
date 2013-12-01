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

#ifndef __Tippi__PlaceMarking__
#define __Tippi__PlaceMarking__

#include "StringUtils.h"

#include <set>
#include <vector>

namespace Tippi {
    class NetNode;
    
    class Marking {
    public:
        typedef std::vector<Marking> List;
        typedef std::set<Marking> Set;
    private:
        std::vector<size_t> m_marking;
    public:
        Marking(const size_t count = 0);
        
        static Marking createMarking(size_t m1);
        static Marking createMarking(size_t m1, size_t m2);
        static Marking createMarking(size_t m1, size_t m2, size_t m3);
        static Marking createMarking(size_t m1, size_t m2, size_t m3, size_t m4);
        static Marking createMarking(size_t m1, size_t m2, size_t m3, size_t m4, size_t m5);
        
        bool operator<(const Marking& rhs) const;
        bool operator==(const Marking& rhs) const;
        int compare(const Marking& rhs) const;
        
        const size_t& operator[](const NetNode* node) const;
        size_t& operator[](const NetNode* node);
        const size_t& operator[](size_t index) const;
        size_t& operator[](size_t index);
        size_t getSize() const;
        
        template <class Translator>
        String asString(const Translator& translate) const {
            StringStream str;
            str << '[';
            for (size_t i = 0; i < m_marking.size() - 1; ++i) {
                translate(str, m_marking[i]);
                str << ',';
            }
            if (!m_marking.empty())
                translate(str, m_marking.back());
            str << "]";
            return str.str();
        }

        String asString() const;
    };
}

#endif /* defined(__Tippi__PlaceMarking__) */
