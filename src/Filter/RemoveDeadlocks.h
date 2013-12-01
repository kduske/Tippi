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

#ifndef __Tippi__RemoveDeadlocks__
#define __Tippi__RemoveDeadlocks__

#include "SharedPointer.h"
#include "Closure.h"

namespace Tippi {
    struct RemoveDeadlocks {
    public:
        typedef std::tr1::shared_ptr<ClAutomaton> ClPtr;
        
        ClPtr operator()(ClPtr automaton) const;
    private:
        ClState::Set findAndMarkPotentialDeadlocks(ClPtr automaton) const;
        ClState::Set findInitialDeadlocks(const ClPtr automaton) const;
        ClState::Set findAdditionalDeadlocks(const ClState::Set& states) const;
        ClState::Set findDeadlockCandidates(const ClState::Set& states) const;
        bool isPotentialDeadlock(const ClState* state) const;
        void markDeadlockDistance(const ClState::Set& states, size_t distance) const;
    };
}

#endif /* defined(__Tippi__RemoveDeadlocks__) */
