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

#ifndef __Tippi__SimpleAutomatonParser__
#define __Tippi__SimpleAutomatonParser__

#include "StringUtils.h"
#include "Tokenizer.h"
#include "Parser.h"

namespace Tippi {
    namespace SimpleAutomatonToken {
        typedef size_t Type;
        static const Type Comma         = 1 <<  0; // 1
        static const Type Semicolon     = 1 <<  1; // 2
        static const Type Automaton     = 1 <<  2; // 4
        static const Type States        = 1 <<  3; // 8
        static const Type Transition    = 1 <<  4; // 16
        static const Type From          = 1 <<  6; // 64
        static const Type To            = 1 <<  7; // 128
        static const Type InitialState  = 1 <<  8; // 256
        static const Type FinalStates   = 1 <<  9; // 512
        static const Type Identifier    = 1 << 10; // 1024
        static const Type Eof           = 1 << 11;
    }
    
    class SimpleAutomatonTokenizer : public Tokenizer<SimpleAutomatonToken::Type> {
    public:
        SimpleAutomatonTokenizer(const char* begin, const char* end);
        SimpleAutomatonTokenizer(const String& str);
    private:
        Token emitToken();
        SimpleAutomatonToken::Type detectType(const char* begin, const char* end) const;
    };
    
    class SimpleAutomaton;
    
    class SimpleAutomatonParser : public Parser<SimpleAutomatonToken::Type> {
    private:
        SimpleAutomatonTokenizer m_tokenizer;
        typedef SimpleAutomatonTokenizer::Token Token;
    public:
        SimpleAutomatonParser(const char* begin, const char* end);
        SimpleAutomatonParser(const String& str);
        
        SimpleAutomaton* parse();
    private:
        void parseStates(SimpleAutomaton& automaton);
        void parseTransition(SimpleAutomaton& automaton);
        void parseInitialState(SimpleAutomaton& automaton);
        void parseFinalStates(SimpleAutomaton& automaton);
        
        template <typename Op>
        void parseStates(Op& op) {
            Token token = m_tokenizer.nextToken();
            expect(SimpleAutomatonToken::Identifier | SimpleAutomatonToken::Semicolon, token);
            
            if (token.type() == SimpleAutomatonToken::Identifier) {
                const String name = token.data();
                op(name, token);
                
                expect(SimpleAutomatonToken::Comma | SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
                while (token.type() == SimpleAutomatonToken::Comma) {
                    expect(SimpleAutomatonToken::Identifier, token = m_tokenizer.nextToken());
                    const String name = token.data();
                    op(name, token);
                    expect(SimpleAutomatonToken::Comma | SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
                }
            }
        }
        
        String tokenName(const SimpleAutomatonToken::Type typeMask) const;
    };
}

#endif /* defined(__Tippi__SimpleAutomatonParser__) */
