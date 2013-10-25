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

#include "IntervalNetParser.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include "Net/IntervalNet.h"
#include "Net/Marking.h"
#include "Net/TimeInterva.h"

namespace Tippi {
    namespace Interval {
        struct TokenMapping {
            const char* str;
            const size_t len;
            const NetToken::Type type;
            
            TokenMapping(const char* i_str, const NetToken::Type i_type) :
            str(i_str),
            len(std::strlen(str)),
            type(i_type) {}
        };
        
        static const TokenMapping TokenMappings[] = {
            TokenMapping("TRANSITION", NetToken::Transition),
            TokenMapping("TIME", NetToken::Time),
            TokenMapping("CONSUME", NetToken::Consume),
            TokenMapping("PRODUCE", NetToken::Produce),
            TokenMapping("MARKING", NetToken::Marking),
            TokenMapping("PLACE", NetToken::Place),
            TokenMapping("SAFE", NetToken::Safe),
            TokenMapping("INPUT", NetToken::InputPlaces),
            TokenMapping("OUTPUT", NetToken::OutputPlaces),
            TokenMapping("FINALMARKING", NetToken::FinalMarking),
            TokenMapping("TIMENET", NetToken::TimeNet)
        };

        NetTokenizer::NetTokenizer(const char* begin, const char* end) :
        Tokenizer(begin, end) {}
        
        NetTokenizer::NetTokenizer(const String& str) :
        Tokenizer(str) {}
        
        NetTokenizer::Token NetTokenizer::emitToken() {
            while (!eof()) {
                size_t startLine = line();
                size_t startColumn = column();
                const char* c = nextChar();

                switch (*c) {
                    case '{':
                        discardUntil("}");
                        break;
                    case ',':
                        return Token(NetToken::Comma, c, c+1, offset(c), startLine, startColumn);
                    case ':':
                        return Token(NetToken::Colon, c, c+1, offset(c), startLine, startColumn);
                    case ';':
                        return Token(NetToken::Semicolon, c, c+1, offset(c), startLine, startColumn);
                    case '(':
                        return Token(NetToken::OParen, c, c+1, offset(c), startLine, startColumn);
                    case ')':
                        return Token(NetToken::CParen, c, c+1, offset(c), startLine, startColumn);
                    case '*':
                        return Token(NetToken::Infinity, c, c+1, offset(c), startLine, startColumn);
                    default:
                        if (isWhitespace(*c)) {
                            discardWhile(Whitespace);
                        } else {
                            const char* begin = c;
                            const char* end = readInteger(begin, Whitespace);
                            if (end > begin)
                                return Token(NetToken::Number, begin, end, offset(begin), startLine, startColumn);
                            
                            end = readDecimal(begin, Whitespace);
                            if (end > begin)
                                return Token(NetToken::Number, begin, end, offset(begin), startLine, startColumn);
                            
                            end = readString(begin, Whitespace);
                            if (end == begin)
                                throw ParserException(startLine, startColumn, "Unexpected character: " + String(c, 1));

                            return Token(detectType(begin, end), begin, end, offset(begin), startLine, startColumn);
                        }
                        break;
                }
            }
            return Token(NetToken::Eof, NULL, NULL, length(), line(), column());
        }

        NetToken::Type NetTokenizer::detectType(const char* begin, const char* end) const {
            assert(end > begin);
            const size_t len = end - begin;

            for (size_t i = 0; i < 11; ++i)
                if (strncmp(begin, TokenMappings[i].str, std::min(len, TokenMappings[i].len)))
                    return TokenMappings[i].type;
            return NetToken::Identifier;
        }
        
        Net* NetParser::parse() {
            expect(NetToken::TimeNet, m_tokenizer.nextToken());
            expect(NetToken::Place, m_tokenizer.nextToken());
            
            Token token = m_tokenizer.nextToken();
            if (token.type() == NetToken::Eof)
                return NULL;
            expect(NetToken::Safe | NetToken::Identifier, token);
            
            Net* net = new Net();
            // parse places
            while (token.type() & (NetToken::Safe | NetToken::Identifier)) {
                m_tokenizer.pushToken(token);
                parsePlaces(*net);
                expect(NetToken::InputPlaces | NetToken::OutputPlaces | NetToken::Safe | NetToken::Identifier | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking, token = m_tokenizer.nextToken());
            }
            
            if (token.type() == NetToken::Eof)
                return net;
            
            expect(NetToken::InputPlaces | NetToken::OutputPlaces | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking, token);
            
            // parse input places
            if (token.type() == NetToken::InputPlaces) {
                parseInputPlaces(*net);
                token = m_tokenizer.nextToken();
            }
            
            if (token.type() == NetToken::Eof)
                return net;
            
            // parse output places
            if (token.type() == NetToken::OutputPlaces) {
                parseOutputPlaces(*net);
                token = m_tokenizer.nextToken();
            }

            if (token.type() == NetToken::Eof)
                return net;
            
            // parse initial marking
            if (token.type() == NetToken::Marking) {
                const Marking initialMarking = parseMarking(*net);
                net->setInitialMarking(initialMarking);
            }
            
            token = m_tokenizer.nextToken();
            if (token.type() == NetToken::Eof)
                return net;
            
            // parse the transitions and arcs
            expect(NetToken::Transition | NetToken::FinalMarking, token);
            if (token.type() == NetToken::Transition) {
                do {
                    parseTransition(*net);
                    token = m_tokenizer.nextToken();
                } while (token.type() == NetToken::Transition);
            }
        }

        void NetParser::parsePlaces(Net& net) {
            long long bound = 0;
            Token token = m_tokenizer.nextToken();
            if (token.type() == NetToken::Safe) {
                expect(NetToken::Number | NetToken::Identifier, token = m_tokenizer.nextToken());
                if (token.type() == NetToken::Number) {
                    bound = token.toInteger<long long>();
                } else {
                    bound = 1;
                    m_tokenizer.pushToken(token);
                }
            } else {
                m_tokenizer.pushToken(token);
            }
            
            if (bound < 0)
                throw ParserException(token.line(), token.column(), "Negative place bound");
            
            while (token.type() != NetToken::Semicolon) {
                expect(NetToken::Identifier, token = m_tokenizer.nextToken());
                net.createPlace(token.data(), static_cast<size_t>(bound));
                expect(NetToken::Comma | NetToken::Semicolon, token = m_tokenizer.nextToken());
            }
        }

        void NetParser::parseInputPlaces(Net& net) {
            parseIOPlaces(net, true);
        }
        
        void NetParser::parseOutputPlaces(Net& net) {
            parseIOPlaces(net, false);
        }

        void NetParser::parseIOPlaces(Net& net, const bool input) {
            Token token = m_tokenizer.nextToken();
            expect(NetToken::Identifier | NetToken::Semicolon, token);
            if (token.type() == NetToken::Identifier) {
                m_tokenizer.pushToken(token);
                do {
                    expect(NetToken::Identifier, token = m_tokenizer.nextToken());
                    Place* place = net.findPlace(token.data());
                    if (place == NULL)
                        throw ParserException(token.line(), token.column(), "Unknown IO place" + token.data());
                    if (input)
                        place->setInputPlace(true);
                    else
                        place->setOutputPlace(true);
                } while ((token = m_tokenizer.nextToken()).type() != NetToken::Semicolon);
            }
        }

        Marking NetParser::parseMarking(Net& net) {
            Marking marking(net.getPlaces().size());
            Token token = m_tokenizer.nextToken();
            expect(NetToken::Identifier | NetToken::Semicolon, token);
            if (token.type() == NetToken::Identifier) {
                m_tokenizer.pushToken(token);
                do {
                    expect(NetToken::Identifier, token = m_tokenizer.nextToken());
                    const String placeName = token.data();
                    
                    expect(NetToken::Colon, token = m_tokenizer.nextToken());
                    expect(NetToken::Number, token = m_tokenizer.nextToken());
                    const long long value = token.toInteger<long long>();
                    
                    if (value < 0)
                        throw ParserException(token.line(), token.column(), "Negative place marking");
                    
                    const Place* place = net.findPlace(placeName);
                    if (place == NULL)
                        throw ParserException(token.line(), token.column(), "Unknown place" + token.data());
                    
                    marking[place] = value;
                } while ((token = m_tokenizer.nextToken()).type() != NetToken::Semicolon);
            }
            expect(NetToken::Semicolon, token);
            return marking;
        }

        void NetParser::parseTransition(Net& net) {
            Token token = m_tokenizer.nextToken();
            expect(NetToken::Identifier, token);
            const String transitionName = token.data();
            
            
            token = m_tokenizer.nextToken();
            if (token.type() == NetToken::Eof)
                return;

            Transition* transition = NULL;
            size_t eft = 0;
            size_t lft = TimeInterval::Infinity;
            
            if (token.type() == NetToken::Time) {
                expect(NetToken::Number | NetToken::Semicolon, token = m_tokenizer.nextToken());
                if (token.type() == NetToken::Number) {
                    const long long e = token.toInteger<long long>();
                    if (e < 0)
                        throw ParserException("Invalid lower time interval bound");
                    eft = static_cast<size_t>(e);
                    
                    expect(NetToken::Comma, token = m_tokenizer.nextToken());
                    expect(NetToken::Number | NetToken::Infinity, token = m_tokenizer.nextToken());
                    
                    if (token.type() == NetToken::Number) {
                        const long long l = token.toInteger<long long>();
                        if (l < 0)
                            throw ParserException("Invalid upper time interval bound");
                        lft = l;
                    }
                }
                
                expect(NetToken::Semicolon, token = m_tokenizer.nextToken());
                token = m_tokenizer.nextToken();
            }
            
            transition = net.createTransition(transitionName, TimeInterval(eft, lft));
            
            if (token.type() == NetToken::Eof)
                return;
            
            // parse the incoming arcs
            if (token.type() == NetToken::Consume) {
            }
            
            if (token.type() == NetToken::Eof)
                return;
            
            // parse the outgoing arcs
            if (token.type() == NetToken::Produce) {
            }
        }

        String NetParser::tokenName(const NetToken::Type typeMask) const {
            StringList names;
            
            if (typeMask | NetToken::Comma)
                names.push_back("','");
            if (typeMask | NetToken::Colon)
                names.push_back("':'");
            if (typeMask | NetToken::Semicolon)
                names.push_back("';'");
            if (typeMask | NetToken::OParen)
                names.push_back("'('");
            if (typeMask | NetToken::CParen)
                names.push_back("')'");
            if (typeMask | NetToken::Infinity)
                names.push_back("'*'");
            if (typeMask | NetToken::Number)
                names.push_back("number");
            if (typeMask | NetToken::Identifier)
                names.push_back("identifier");
            
            for (size_t i = 0; i < 11; ++i)
                if (typeMask | TokenMappings[i].type)
                    names.push_back("'" + String(TokenMappings[i].str, TokenMappings[i].len) + "'");
            if (names.empty())
                return "unknown token type";
            if (names.size() == 1)
                return names[0];
            
            return StringUtils::join(names, ", ");
        }
    }
}
