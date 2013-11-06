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

#include <vector>

namespace Tippi {
    class NetNode;
    
    class Marking {
    public:
        typedef std::vector<Marking> List;
    private:
        std::vector<size_t> m_marking;
    public:
        Marking(const size_t count = 0);
        static Marking createMarking(const size_t m0, ...);
        
        bool operator<(const Marking& rhs) const;
        bool operator==(const Marking& rhs) const;
        int compare(const Marking& rhs) const;
        
        const size_t& operator[](const NetNode* node) const;
        size_t& operator[](const NetNode* node);
        const size_t& operator[](const size_t index) const;
        size_t& operator[](const size_t index);
        size_t getSize() const;
        
        String asString() const;
    };
}

#endif /* defined(__Tippi__PlaceMarking__) */
