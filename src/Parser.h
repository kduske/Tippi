/*
 Copyright (C) 2013-2014 Kristian Duske
 
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

#ifndef __Tippi__Parser__
#define __Tippi__Parser__

#include "Exceptions.h"
#include "StringUtils.h"
#include "Token.h"

namespace Tippi {
    template <typename TokenType>
    class Parser {
    private:
        typedef TokenTemplate<TokenType> Token;
    public:
        virtual ~Parser() {}
    protected:
        void expect(const TokenType typeMask, const Token& token) const {
            if ((token.type() & typeMask) == 0)
                throw ParserException(token.line(), token.column()) << "Expected " << tokenName(typeMask) << ", but got " << tokenName(token.type());
        }
    private:
        virtual String tokenName(const TokenType typeMask) const = 0;
    };
}

#endif /* defined(__Tippi__Parser__) */
