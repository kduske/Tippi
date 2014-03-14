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

#include "SimpleAutomatonParser.h"

#include "SimpleAutomaton.h"

namespace Tippi {
    struct SimpleAutomatonTokenMapping {
        const char* str;
        const size_t len;
        const SimpleAutomatonToken::Type type;
        
        SimpleAutomatonTokenMapping(const char* i_str, const SimpleAutomatonToken::Type i_type) :
        str(i_str),
        len(std::strlen(str)),
        type(i_type) {}
    };
    
    static const SimpleAutomatonTokenMapping SimpleAutomatonTokenMappings[] = {
        SimpleAutomatonTokenMapping("COMMA", SimpleAutomatonToken::Comma),
        SimpleAutomatonTokenMapping("SEMICOLON", SimpleAutomatonToken::Semicolon),
        SimpleAutomatonTokenMapping("AUTOMATON", SimpleAutomatonToken::Automaton),
        SimpleAutomatonTokenMapping("STATES", SimpleAutomatonToken::States),
        SimpleAutomatonTokenMapping("TRANSITION", SimpleAutomatonToken::Transition),
        SimpleAutomatonTokenMapping("FROM", SimpleAutomatonToken::From),
        SimpleAutomatonTokenMapping("TO", SimpleAutomatonToken::To),
        SimpleAutomatonTokenMapping("INITIALSTATE", SimpleAutomatonToken::InitialState),
        SimpleAutomatonTokenMapping("FINALSTATES", SimpleAutomatonToken::FinalStates)
    };

    SimpleAutomatonTokenizer::SimpleAutomatonTokenizer(const char* begin, const char* end) :
    Tokenizer(begin, end) {}
    
    SimpleAutomatonTokenizer::SimpleAutomatonTokenizer(const String& str) :
    Tokenizer(str) {}

    SimpleAutomatonTokenizer::Token SimpleAutomatonTokenizer::emitToken() {
        while (!eof()) {
            size_t startLine = line();
            size_t startColumn = column();
            const char* c = curPos();
            
            switch (*c) {
                case '{':
                    discardUntil("}");
                    break;
                case ',':
                    advance();
                    return Token(SimpleAutomatonToken::Comma, c, c+1, offset(c), startLine, startColumn);
                case ';':
                    advance();
                    return Token(SimpleAutomatonToken::Semicolon, c, c+1, offset(c), startLine, startColumn);
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    discardWhile(Whitespace);
                    break;
                default: {
                    const char* e = readString(Whitespace + ";,:");
                    if (e == NULL)
                        throw ParserException(startLine, startColumn, "Unexpected character: " + String(c, 1));
                    return Token(detectType(c, e), c, e, offset(c), startLine, startColumn);
                }
            }
        }
        return Token(SimpleAutomatonToken::Eof, NULL, NULL, length(), line(), column());
    }
    
    SimpleAutomatonToken::Type SimpleAutomatonTokenizer::detectType(const char* begin, const char* end) const {
        assert(end > begin);
        const size_t len = end - begin;
        
        for (size_t i = 0; i < 9; ++i)
            if (len == SimpleAutomatonTokenMappings[i].len &&
                std::strncmp(begin, SimpleAutomatonTokenMappings[i].str, len) == 0)
                return SimpleAutomatonTokenMappings[i].type;
        return SimpleAutomatonToken::Identifier;
    }

    SimpleAutomatonParser::SimpleAutomatonParser(const char* begin, const char* end) :
    m_tokenizer(begin, end) {}
    
    SimpleAutomatonParser::SimpleAutomatonParser(const String& str) :
    m_tokenizer(str) {}
    
    SimpleAutomaton* SimpleAutomatonParser::parse() {
        Token token;
        expect(SimpleAutomatonToken::Automaton | SimpleAutomatonToken::Eof, token = m_tokenizer.nextToken());
        if (token.type() == SimpleAutomatonToken::Eof)
            return NULL;
        
        SimpleAutomaton* automaton = new SimpleAutomaton();
        try {
            const SimpleAutomatonToken::Type allowed = SimpleAutomatonToken::States | SimpleAutomatonToken::Transition | SimpleAutomatonToken::InitialState |SimpleAutomatonToken::FinalStates | SimpleAutomatonToken::Eof;
            expect(allowed, token = m_tokenizer.nextToken());
            
            while (token.type() != SimpleAutomatonToken::Eof) {
                if (token.type() == SimpleAutomatonToken::States)
                    parseStates(*automaton);
                else if (token.type() == SimpleAutomatonToken::Transition)
                    parseTransition(*automaton);
                else if (token.type() == SimpleAutomatonToken::InitialState)
                    parseInitialState(*automaton);
                else if (token.type() == SimpleAutomatonToken::FinalStates)
                    parseFinalStates(*automaton);
                expect(allowed, token = m_tokenizer.nextToken());
            }
        } catch (...) {
            delete automaton;
            throw;
        }
        
        return automaton;
    }

    struct StateParserOp {
        SimpleAutomaton& automaton;
        
        StateParserOp(SimpleAutomaton& i_automaton) :
        automaton(i_automaton) {}
        
        void operator()(const String& name, const SimpleAutomatonTokenizer::Token& token) {
            try {
                automaton.createState(name);
            } catch (const AutomatonException& e) {
                throw ParserException(token.line(), token.column(), e.what());
            }
        }
    };
    
    void SimpleAutomatonParser::parseStates(SimpleAutomaton& automaton) {
        StateParserOp op(automaton);
        parseStates(op);
    }
    
    void SimpleAutomatonParser::parseTransition(SimpleAutomaton& automaton) {
        Token token = m_tokenizer.nextToken();

        expect(SimpleAutomatonToken::Identifier, token);
        const String label = token.data();
        expect(SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
        
        expect(SimpleAutomatonToken::From, token = m_tokenizer.nextToken());
        expect(SimpleAutomatonToken::Identifier, token = m_tokenizer.nextToken());
        const String fromName = token.data();
        SimpleAutomatonState* fromState = automaton.findState(fromName);
        if (fromState == NULL)
            throw ParserException(token.line(), token.column(), "Unknown state " + fromName);
        expect(SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
        
        expect(SimpleAutomatonToken::To, token = m_tokenizer.nextToken());
        expect(SimpleAutomatonToken::Identifier, token = m_tokenizer.nextToken());
        const String toName = token.data();
        SimpleAutomatonState* toState = automaton.findState(toName);
        if (fromState == NULL)
            throw ParserException(token.line(), token.column(), "Unknown state " + toName);
        expect(SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
        
        if (label.empty())
            automaton.connectWithUnobservableEdge(fromState, toState);
        else
            automaton.connectWithObservableEdge(fromState, toState, label);
    }
    
    void SimpleAutomatonParser::parseInitialState(SimpleAutomaton& automaton) {
        Token token = m_tokenizer.nextToken();
        expect(SimpleAutomatonToken::Identifier, token);
        
        const String name = token.data();
        SimpleAutomatonState* state = automaton.findState(name);
        if (state == NULL)
            throw ParserException(token.line(), token.column(), "Unknown initial state " + name);
        automaton.setInitialState(state);
        expect(SimpleAutomatonToken::Semicolon, token = m_tokenizer.nextToken());
    }

    struct FinalStateParserOp {
        SimpleAutomaton& automaton;
        
        FinalStateParserOp(SimpleAutomaton& i_automaton) :
        automaton(i_automaton) {}
        
        void operator()(const String& name, const SimpleAutomatonTokenizer::Token& token) {
            SimpleAutomatonState* state = automaton.findState(name);
            if (state == NULL)
                throw ParserException(token.line(), token.column(), "Unknown final state " + name);
            if (state->isFinal())
                throw ParserException(token.line(), token.column(), "Duplicate final state " + name);
            state->setFinal(true);
            automaton.addFinalState(state);
        }
    };
    
    void SimpleAutomatonParser::parseFinalStates(SimpleAutomaton& automaton) {
        FinalStateParserOp op(automaton);
        parseStates(op);
    }

    String SimpleAutomatonParser::tokenName(const SimpleAutomatonToken::Type typeMask) const {
        StringList names;
        
        if (typeMask | SimpleAutomatonToken::Comma)
            names.push_back("','");
        if (typeMask | SimpleAutomatonToken::Semicolon)
            names.push_back("';'");
        if (typeMask | SimpleAutomatonToken::Identifier)
            names.push_back("identifier");
        
        for (size_t i = 0; i < 11; ++i)
            if (typeMask | SimpleAutomatonTokenMappings[i].type)
                names.push_back("'" + String(SimpleAutomatonTokenMappings[i].str, SimpleAutomatonTokenMappings[i].len) + "'");
        if (names.empty())
            return "unknown token type";
        if (names.size() == 1)
            return names[0];
        
        return StringUtils::join(names, ", ");
    }
}
