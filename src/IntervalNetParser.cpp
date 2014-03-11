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

#include "IntervalNetParser.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include "IntervalNet.h"
#include "Marking.h"
#include "TimeInterval.h"

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
                const char* c = curPos();

                switch (*c) {
                    case '{':
                        discardUntil("}");
                        break;
                    case ',':
                        advance();
                        return Token(NetToken::Comma, c, c+1, offset(c), startLine, startColumn);
                    case ':':
                        advance();
                        return Token(NetToken::Colon, c, c+1, offset(c), startLine, startColumn);
                    case ';':
                        advance();
                        return Token(NetToken::Semicolon, c, c+1, offset(c), startLine, startColumn);
                    case '(':
                        advance();
                        return Token(NetToken::OParen, c, c+1, offset(c), startLine, startColumn);
                    case ')':
                        advance();
                        return Token(NetToken::CParen, c, c+1, offset(c), startLine, startColumn);
                    case '*':
                        advance();
                        return Token(NetToken::Infinity, c, c+1, offset(c), startLine, startColumn);
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        discardWhile(Whitespace);
                        break;
                    default: {
                        const char* e = readInteger(Whitespace + ";,:");
                        if (e != NULL)
                            return Token(NetToken::Number, c, e, offset(c), startLine, startColumn);
                        e = readDecimal(Whitespace + ";,:");
                        if (e != NULL)
                            return Token(NetToken::Number, c, e, offset(c), startLine, startColumn);
                        e = readString(Whitespace + ";,:");
                        if (e == NULL)
                            throw ParserException(startLine, startColumn, "Unexpected character: " + String(c, 1));
                        return Token(detectType(c, e), c, e, offset(c), startLine, startColumn);
                    }
                }
            }
            return Token(NetToken::Eof, NULL, NULL, length(), line(), column());
        }

        NetToken::Type NetTokenizer::detectType(const char* begin, const char* end) const {
            assert(end > begin);
            const size_t len = end - begin;

            for (size_t i = 0; i < 11; ++i)
                if (len == TokenMappings[i].len &&
                    std::strncmp(begin, TokenMappings[i].str, len) == 0)
                    return TokenMappings[i].type;
            return NetToken::Identifier;
        }
        
        NetParser::NetParser(const char* begin, const char* end) :
        m_tokenizer(begin, end) {}
        
        NetParser::NetParser(const String& str) :
        m_tokenizer(str) {}

        Net* NetParser::parse() {
            Token token;
            Net* net = new Net();

            expect(NetToken::TimeNet | NetToken::Eof, token = m_tokenizer.nextToken());
            if (token.type() == NetToken::Eof)
                return net;
            
            expect(NetToken::Place | NetToken::InputPlaces | NetToken::OutputPlaces | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token = m_tokenizer.nextToken());

            if (token.type() == NetToken::Place) {
                expect(NetToken::Safe | NetToken::Identifier, token = m_tokenizer.nextToken());

                // parse places
                while (token.type() & (NetToken::Safe | NetToken::Identifier)) {
                    m_tokenizer.pushToken(token);
                    parsePlaces(*net);
                    expect(NetToken::InputPlaces | NetToken::OutputPlaces | NetToken::Safe | NetToken::Identifier | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token = m_tokenizer.nextToken());
                }
            }

            expect(NetToken::InputPlaces | NetToken::OutputPlaces | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token);
            
            // parse input places
            if (token.type() == NetToken::InputPlaces) {
                parseInputPlaces(*net);
                token = m_tokenizer.nextToken();
            }
            
            expect(NetToken::OutputPlaces | NetToken::Marking | NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token);
            
            // parse output places
            if (token.type() == NetToken::OutputPlaces) {
                parseOutputPlaces(*net);
                token = m_tokenizer.nextToken();
            }
            
            expect(NetToken::Marking | NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token);
            
            // parse initial marking
            if (token.type() == NetToken::Marking) {
                const Marking initialMarking = parseMarking(*net);
                net->setInitialMarking(initialMarking);
                token = m_tokenizer.nextToken();
            }
            
            expect(NetToken::Transition | NetToken::FinalMarking | NetToken::Eof, token);
            
            // parse the transitions and arcs
            if (token.type() == NetToken::Transition) {
                do {
                    parseTransition(*net);
                    token = m_tokenizer.nextToken();
                } while (token.type() == NetToken::Transition);
            }
            
            expect(NetToken::FinalMarking | NetToken::Eof, token);
            
            while (token.type() == NetToken::FinalMarking) {
                const Marking finalMarking = parseMarking(*net);
                net->addFinalMarking(finalMarking);
                expect(NetToken::FinalMarking | NetToken::Eof, token = m_tokenizer.nextToken());
            }
            
            expect(NetToken::Eof, token);
            return net;
        }

        void NetParser::parsePlaces(Net& net) {
            long long bound = 0;
            Token token = m_tokenizer.nextToken();
            if (token.type() == NetToken::Safe) {
                expect(NetToken::Number | NetToken::Identifier, token = m_tokenizer.nextToken());
                if (token.type() == NetToken::Number) {
                    bound = token.toInteger<long long>();
                    expect(NetToken::Colon, token = m_tokenizer.nextToken());
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
                        throw ParserException(token.line(), token.column(), "Invalid lower time interval bound");
                    eft = static_cast<size_t>(e);
                    
                    expect(NetToken::Comma, token = m_tokenizer.nextToken());
                    expect(NetToken::Number | NetToken::Infinity, token = m_tokenizer.nextToken());
                    
                    if (token.type() == NetToken::Number) {
                        const long long l = token.toInteger<long long>();
                        if (l < 0)
                            throw ParserException(token.line(), token.column(), "Invalid upper time interval bound");
                        lft = l;
                    }
                }
                
                expect(NetToken::Semicolon, token = m_tokenizer.nextToken());
                token = m_tokenizer.nextToken();
            }
            
            transition = net.createTransition(transitionName, TimeInterval(eft, lft));
            
            // parse the incoming arcs
            if (token.type() == NetToken::Consume) {
                expect(NetToken::Identifier | NetToken::Semicolon, token = m_tokenizer.nextToken());
                if (token.type() == NetToken::Identifier) {
                    m_tokenizer.pushToken(token);
                    do {
                        parseIncomingArc(net, *transition);
                    } while (m_tokenizer.nextToken().type() != NetToken::Semicolon);
                }
                token = m_tokenizer.nextToken();
            }
            
            // parse the outgoing arcs
            if (token.type() == NetToken::Produce) {
                expect(NetToken::Identifier | NetToken::Semicolon, token = m_tokenizer.nextToken());
                if (token.type() == NetToken::Identifier) {
                    m_tokenizer.pushToken(token);
                    do {
                        parseOutgoingArc(net, *transition);
                    } while (m_tokenizer.nextToken().type() != NetToken::Semicolon);
                }
            } else {
                m_tokenizer.pushToken(token);
            }
        }
        
        void NetParser::parseIncomingArc(Net& net, Transition& transition) {
            parseArc(net, transition, true);
        }
        
        void NetParser::parseOutgoingArc(Net& net, Transition& transition) {
            parseArc(net, transition, false);
        }
        
        void NetParser::parseArc(Net& net, Transition& transition, const bool incoming) {
            Token token = m_tokenizer.nextToken();
            expect(NetToken::Identifier, token);
            const String placeName = token.data();

            Place* place = net.findPlace(placeName);
            if (place == NULL)
                throw ParserException(token.line(), token.column(), "Unknown place :'" + placeName + "'");

            expect(NetToken::Colon, token = m_tokenizer.nextToken());
            expect(NetToken::Number, token = m_tokenizer.nextToken());
            const long long multiplicity = token.toInteger<long long>();
            if (multiplicity != 1)
                throw ParserException(token.line(), token.column(), "Arc multiplicity must be 1");

            if (incoming)
                net.connect(place, &transition);
            else
                net.connect(&transition, place);
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
