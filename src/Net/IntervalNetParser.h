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

#ifndef __Tippi__IntervalNetParser__
#define __Tippi__IntervalNetParser__

#include "StringUtils.h"
#include "Tokenizer.h"
#include "Parser.h"

namespace Tippi {
    class Marking;

    namespace Interval {
        namespace NetToken {
            typedef size_t Type;
            static const Type Comma        = 1 <<  0; // 1
            static const Type Colon        = 1 <<  1; // 2
            static const Type Semicolon    = 1 <<  2; // 4
            static const Type OParen       = 1 <<  3; // 8
            static const Type CParen       = 1 <<  4; // 16
            static const Type TimeNet      = 1 <<  6; // 64
            static const Type Place        = 1 <<  7; // 128
            static const Type Safe         = 1 <<  8; // 256
            static const Type InputPlaces  = 1 <<  9; // 512
            static const Type OutputPlaces = 1 << 10; // 1024
            static const Type Marking      = 1 << 11; // 2048
            static const Type Transition   = 1 << 12; // 4096
            static const Type Consume      = 1 << 13; // 8192
            static const Type Produce      = 1 << 14; // 16384
            static const Type Time         = 1 << 15; // 32768
            static const Type FinalMarking = 1 << 16; // 65536
            static const Type Number       = 1 << 17; // 131072
            static const Type Identifier   = 1 << 18; // 262144
            static const Type Infinity     = 1 << 19;
            static const Type Eof          = 1 << 20;
        }

        class NetTokenizer : public Tokenizer<NetToken::Type> {
        public:
            NetTokenizer(const char* begin, const char* end);
            NetTokenizer(const String& str);
        private:
            Token emitToken();
            NetToken::Type detectType(const char* begin, const char* end) const;
        };
        
        class Net;
        
        class NetParser : public Parser<NetToken::Type> {
        private:
            NetTokenizer m_tokenizer;
            typedef NetTokenizer::Token Token;
        public:
            NetParser(const char* begin, const char* end);
            NetParser(const String& str);
            
            Net* parse();
        private:
            void parsePlaces(Net& net);
            void parseInputPlaces(Net& net);
            void parseOutputPlaces(Net& net);
            void parseIOPlaces(Net& net, const bool input);
            Marking parseMarking(Net& net);
            void parseTransition(Net& net);
            String tokenName(const NetToken::Type typeMask) const;
        };
    }
}

#endif /* defined(__Tippi__IntervalNetParser__) */
