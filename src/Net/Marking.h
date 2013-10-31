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

#include <vector>

namespace Tippi {
    class NetNode;
    
    class Marking {
    public:
        typedef std::vector<Marking> List;
    private:
        std::vector<size_t> m_marking;
    public:
        Marking(const size_t placeCount = 0);
        
        const size_t& operator[](const NetNode* node) const;
        size_t& operator[](const NetNode* node);
        size_t getSize() const;
    };
}

#endif /* defined(__Tippi__PlaceMarking__) */
