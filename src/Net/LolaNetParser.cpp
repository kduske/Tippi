//
//  LolaNetParser.cpp
//  Tippi
//
//  Created by Kristian Duske on 29.08.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#include "LolaNetParser.h"

/*
#include "Net.h"
#include "Place.h"
#include "PlaceMarking.h"
#include "Transition.h"
#include "Arc.h"

#include <limits>

namespace Tippi {
    namespace Lola {
        Tokenizer::Tokenizer(std::istream& stream) :
        m_stream(stream),
        m_line(1),
        m_position(0) {
            std::ios::pos_type pos = m_stream.tellg();
            m_stream.seekg(0, std::ios::end);
            m_length = m_stream.tellg() - pos;
            m_stream.seekg(pos, std::ios::beg);
        }
        
        Tokenizer::~Tokenizer() {
            while (!m_tokenStack.empty()) delete m_tokenStack.back(), m_tokenStack.pop_back();
        }

        Token::Ptr Tokenizer::token(const std::string& data, bool number) {
            if (data == "NET")
                return Token::Ptr(new Token(TokenTypes::Net, m_line, m_position));
            if (data == "TIMENET")
                return Token::Ptr(new Token(TokenTypes::TimeNet, m_line, m_position));
            if (data == "PLACE")
                return Token::Ptr(new Token(TokenTypes::Place, m_line, m_position));
            if (data == "SAFE")
                return Token::Ptr(new Token(TokenTypes::Safe, m_line, m_position));
            if (data == "INPUT")
                return Token::Ptr(new Token(TokenTypes::InputPlaces, m_line, m_position));
            if (data == "OUTPUT")
                return Token::Ptr(new Token(TokenTypes::OutputPlaces, m_line, m_position));
            if (data == "MARKING")
                return Token::Ptr(new Token(TokenTypes::Marking, m_line, m_position));
            if (data == "TRANSITION")
                return Token::Ptr(new Token(TokenTypes::Transition, m_line, m_position));
            if (data == "CONSUME")
                return Token::Ptr(new Token(TokenTypes::Consume, m_line, m_position));
            if (data == "PRODUCE")
                return Token::Ptr(new Token(TokenTypes::Produce, m_line, m_position));
            if (data == "TIME")
                return Token::Ptr(new Token(TokenTypes::Time, m_line, m_position));
            if (data == "FINALMARKING")
                return Token::Ptr(new Token(TokenTypes::FinalMarking, m_line, m_position));
            if (number)
                return Token::Ptr(new Token(TokenTypes::Number, m_line, m_position, data));
            return Token::Ptr(new Token(TokenTypes::Identifier, m_line, m_position, data));
        }

        Token::Ptr Tokenizer::token(TokenType type) {
            return Token::Ptr(new Token(type, m_line, m_position));
        }

        Token::Ptr Tokenizer::nextToken() {
            if (!m_tokenStack.empty()) {
                Token* token = m_tokenStack.back();
                m_tokenStack.pop_back();
                return Token::Ptr(token);
            }
            
            m_buffer.str("");
            bool empty = true;
            bool number = false;
            bool comment = false;
            
            while (!eof()) {
                char c = nextChar();
                if (comment) {
                    if (c == '}')
                        comment = false;
                    continue;
                }
                
                switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                        if (!empty)
                            return token(m_buffer.str(), number);
                        break;
                    case '{':
                        comment = true;
                        break;
                    case ',':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::Comma);
                    case ':':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::Colon);
                    case ';':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::Semicolon);
                    case '(':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::OParen);
                    case ')':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::CParen);
                    case '*':
                        if (!empty) {
                            putChar(c);
                            return token(m_buffer.str(), number);
                        }
                        return token(TokenTypes::Infinity);
                    case '+':
                    case '-':
                        if (!empty && number)
                            number = false;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (empty && !number)
                            number = true;
                        m_buffer << c;
                        empty = false;
                        break;
                    default:
                        m_buffer << c;
                        empty = false;
                }
            }
            
            return Token::Ptr();
        }

        Token::Ptr Tokenizer::peekToken() {
            size_t line = m_line;
            size_t position = m_position;
            
            Token::Ptr token = nextToken();
            m_line = line;
            m_position = position;
            m_stream.seekg(position, std::ios::beg);
            m_stream.clear();

            return token;
        }

        
        void Tokenizer::pushToken(Token* token) {
            m_tokenStack.push_back(new Token(*token));
        }
        
        std::string Parser::tokenNames(TokenType types) {
            std::vector<std::string> names;
            if (types | TokenTypes::Comma)
                names.push_back("','");
            if (types | TokenTypes::Colon)
                names.push_back("':'");
            if (types | TokenTypes::Semicolon)
                names.push_back("';'");
            if (types | TokenTypes::OParen)
                names.push_back("'('");
            if (types | TokenTypes::CParen)
                names.push_back("')'");
            if (types | TokenTypes::Net)
                names.push_back("'NET'");
            if (types | TokenTypes::TimeNet)
                names.push_back("'TIMENET'");
            if (types | TokenTypes::Place)
                names.push_back("'PLACE'");
            if (types | TokenTypes::Safe)
                names.push_back("'SAFE'");
            if (types | TokenTypes::InputPlaces)
                names.push_back("'INPUT'");
            if (types | TokenTypes::OutputPlaces)
                names.push_back("'OUTPUT'");
            if (types | TokenTypes::Marking)
                names.push_back("'MARKING'");
            if (types | TokenTypes::Transition)
                names.push_back("'TRANSITION'");
            if (types | TokenTypes::Consume)
                names.push_back("'CONSUME'");
            if (types | TokenTypes::Produce)
                names.push_back("'PRODUCE'");
            if (types | TokenTypes::Time)
                names.push_back("'TIME'");
            if (types | TokenTypes::FinalMarking)
                names.push_back("'FINALMARKING'");
            if (types | TokenTypes::Number)
                names.push_back("number");
            if (types | TokenTypes::Identifier)
                names.push_back("identifier");
            if (types | TokenTypes::Infinity)
                names.push_back("infinity");
            
            if (names.empty())
                return "unknown token type";
            if (names.size() == 1)
                return names[0];
            
            std::stringstream str;
            str << names[0];
            for (unsigned int i = 1; i < names.size() - 1; i++)
                str << ", " << names[i];
            str << ", or " << names[names.size() - 1];
            return str.str();
        }

        void Parser::expect(TokenType expected, Token* actual) {
            if ((expected & actual->type()) == 0)
                throw ParserException(actual->line(), "Expected " + tokenNames(expected));
        }

        void Parser::parsePlaces(Net& net) {
            long bound = 0;
            Token::Ptr token = m_tokenizer.nextToken();
            if (token->type() == TokenTypes::Safe) {
                expect(TokenTypes::Number | TokenTypes::Identifier, (token = m_tokenizer.nextToken()).get());
                if (token->type() == TokenTypes::Number) {
                    bound = token->number();
                } else {
                    bound = 1;
                    m_tokenizer.pushToken(token.get());
                }
            } else {
                m_tokenizer.pushToken(token.get());
            }
            
            if (bound < 0) // TODO warning: negative bound
                bound = 0;  // unbounded
            else if (bound > std::numeric_limits<unsigned int>::max()) // TODO warning: out of bounds, haha
                bound = 0; // unbounded
            
            while (token->type() != TokenTypes::Semicolon) {
                expect(TokenTypes::Identifier, (token = m_tokenizer.nextToken()).get());
                
                const std::string& name = token->data();
                net.createPlace(name, static_cast<unsigned int>(bound));
                
                expect(TokenTypes::Comma | TokenTypes::Semicolon, (token = m_tokenizer.nextToken()).get());
            }
        }

        void Parser::parseIOPlaces(Net& net, bool input) {
            Token::Ptr token = m_tokenizer.nextToken();
            expect(TokenTypes::Identifier | TokenTypes::Semicolon, token.get());
            if (token->type() == TokenTypes::Identifier) {
                m_tokenizer.pushToken(token.get());
                do {
                    expect(TokenTypes::Identifier, (token = m_tokenizer.nextToken()).get());
                    const std::string& name = token->data();
                    Place* place = net.place(name);
                    if (place == NULL) {
                        // TODO warning: unspecified input place
                    } else {
                        if (input)
                            place->setInputPlace(true);
                        else
                            place->setOutputPlace(true);
                    }
                } while ((token = m_tokenizer.nextToken())->type() != TokenTypes::Semicolon);
            }
        }

        PlaceMarking Parser::parseMarking(Net& net) {
            PlaceMarking placeMarking(net.places());
            Token::Ptr token = m_tokenizer.nextToken();
            expect(TokenTypes::Identifier | TokenTypes::Semicolon, token.get());
            if (token->type() == TokenTypes::Identifier) {
                m_tokenizer.pushToken(token.get());
                do {
                    expect(TokenTypes::Identifier, (token = m_tokenizer.nextToken()).get());
                    std::string name = token->data();
                    expect(TokenTypes::Colon, (token = m_tokenizer.nextToken()).get());
                    expect(TokenTypes::Number, (token = m_tokenizer.nextToken()).get());
                    long marking = token->number();
                    
                    if (marking < 0) // TODO warning:: negative marking
                        marking = 0; // unmarked
                    else if (marking > std::numeric_limits<unsigned int>::max()) // TODO warning: marking out of bounds
                        marking = std::numeric_limits<unsigned int>::max();
                    
                    const Place* place = net.place(name);
                    if (place != NULL) {
                        placeMarking[*place] = static_cast<unsigned int>(marking);
                    } else {
                        // TODO warning: marking for unknown place
                    }
                } while ((token = m_tokenizer.nextToken())->type() != TokenTypes::Semicolon);
            }
            
            expect(TokenTypes::Semicolon, token.get());
            return placeMarking;
        }

        void Parser::parseTransition(Net& net) {
            Token::Ptr token = m_tokenizer.nextToken();
            expect(TokenTypes::Identifier, token.get());
            std::string transitionName = token->data();
            Transition* transition = NULL;
            
            long eft = 0;
            long lft = std::numeric_limits<unsigned int>::max();
            
            token = m_tokenizer.nextToken();
            if (token.get() != NULL) {
                if (token->type() == TokenTypes::Time) {
                    expect(TokenTypes::Number | TokenTypes::Semicolon, (token = m_tokenizer.nextToken()).get());
                    if (token->type() == TokenTypes::Number) {
                        eft = token->number();
                        if (eft < 0) {
                            // TODO warning: negative eft
                            eft = 0;
                        } else if (eft > std::numeric_limits<unsigned int>::max()) {
                            // TODO warning: eft out of bounds
                            eft = std::numeric_limits<unsigned int>::max();
                        }
                        
                        expect(TokenTypes::Comma, (token = m_tokenizer.nextToken()).get());
                        expect(TokenTypes::Number | TokenTypes::Infinity, (token = m_tokenizer.nextToken()).get());
                        
                        if (token->type() == TokenTypes::Number) {
                            lft = token->number();
                            if (lft < 0) {
                                // TODO warning: negative lft
                                lft = 0;
                            } else if (lft > std::numeric_limits<unsigned int>::max()) {
                                // TODO warning: lft out of bounds
                                lft = std::numeric_limits<unsigned int>::max();
                            }
                            
                            if (lft < eft) {
                                // TODO warning: lft < eft
                                lft = eft;
                            }
                        } else {
                            lft = Transition::Infinite;
                        }
                        
                        expect(TokenTypes::Semicolon, (token = m_tokenizer.nextToken()).get());
                        token = m_tokenizer.nextToken();
                    }
                }
                
                transition = net.createTransition(transitionName, static_cast<unsigned int>(eft), static_cast<unsigned int>(lft));
                
                if (token.get() != NULL) {
                    if (token->type() == TokenTypes::Consume) {
                        expect(TokenTypes::Identifier | TokenTypes::Semicolon, (token = m_tokenizer.nextToken()).get());
                        if (token->type() == TokenTypes::Identifier) {
                            m_tokenizer.pushToken(token.get());
                            do {
                                parseArc(net, *transition, true);
                            } while ((token = m_tokenizer.nextToken())->type() != TokenTypes::Semicolon);
                        }
                        token = m_tokenizer.nextToken();
                    }
                    
                    if (token.get() != NULL) {
                        if (token->type() == TokenTypes::Produce) {
                            expect(TokenTypes::Identifier | TokenTypes::Semicolon, (token = m_tokenizer.nextToken()).get());
                            if (token->type() == TokenTypes::Identifier) {
                                m_tokenizer.pushToken(token.get());
                                do {
                                    parseArc(net, *transition, false);
                                } while ((token = m_tokenizer.nextToken())->type() != TokenTypes::Semicolon);
                            }
                            token = m_tokenizer.nextToken();
                        }
                        
                    }
                }

                m_tokenizer.pushToken(token.get());
            }
        }
        
        void Parser::parseArc(Net& net, Transition& transition, bool incoming) {
            Token::Ptr token = m_tokenizer.nextToken();
            expect(TokenTypes::Identifier, token.get());
            std::string placeName = token->data();
            expect(TokenTypes::Colon, (token = m_tokenizer.nextToken()).get());
            expect(TokenTypes::Number, (token = m_tokenizer.nextToken()).get());
            long multiplicity = token->number();
            
            if (multiplicity < 0) {
                // TODO warning: negative multiplicity
                multiplicity = 0;
            } else if (multiplicity > std::numeric_limits<unsigned int>::max()) {
                // TODO warning: multiplicity out of bounds
                multiplicity = std::numeric_limits<unsigned int>::max();
            }
            
            if (multiplicity > 0) {
                Place* place = net.place(placeName);
                if (place != NULL) {
                    if (incoming)
                        net.connect(place, &transition, static_cast<unsigned int>(multiplicity));
                    else
                        net.connect(&transition, place, static_cast<unsigned int>(multiplicity));
                } else {
                    // TODO warning: unknown input place
                }
            } else {
                // TODO warning: skipping arc due to multiplicity 0
            }
        }

        Parser::Parser(std::istream& stream) :
        m_tokenizer(stream) {}
        
        Net* Parser::parseTimeNet() {
            Token::Ptr token = m_tokenizer.nextToken();
            if (token.get() == NULL)
                return NULL;
            
            expect(TokenTypes::TimeNet, token.get());
            
            expect(TokenTypes::Place, (token = m_tokenizer.nextToken()).get());
            expect(TokenTypes::Safe | TokenTypes::Identifier, (token = m_tokenizer.nextToken()).get());

            Net* net = new Net();

            // parse the places
            while (token->type() & (TokenTypes::Safe | TokenTypes::Identifier)) {
                m_tokenizer.pushToken(token.get());
                parsePlaces(*net);
                expect(TokenTypes::InputPlaces | TokenTypes::OutputPlaces | TokenTypes::Safe | TokenTypes::Identifier | TokenTypes::Marking | TokenTypes::Transition | TokenTypes::FinalMarking, (token = m_tokenizer.nextToken()).get());
            }
            
            if (token.get() != NULL) {
                expect(TokenTypes::InputPlaces | TokenTypes::OutputPlaces | TokenTypes::Marking | TokenTypes::Transition | TokenTypes::FinalMarking, token.get());
                
                // parse the input places
                if (token->type() == TokenTypes::InputPlaces) {
                    parseIOPlaces(*net, true);
                    token = m_tokenizer.nextToken();
                }
                
                if (token.get() != NULL) {
                    // parse the output places
                    if (token->type() == TokenTypes::OutputPlaces) {
                        parseIOPlaces(*net, false);
                        token = m_tokenizer.nextToken();
                    }
                    
                    if (token.get() != NULL) {
                        // parse the initial marking
                        if (token->type() == TokenTypes::Marking) {
                            PlaceMarking initialMarking = parseMarking(*net);
                            net->setInitialPlaceMarking(initialMarking);
                        }
                        
                        token = m_tokenizer.nextToken();
                        if (token.get() != NULL) {
                            expect(TokenTypes::Transition | TokenTypes::FinalMarking, token.get());
                            
                            if (token->type() == TokenTypes::Transition) {
                                // parse the transitions
                                do {
                                    parseTransition(*net);
                                    token = m_tokenizer.nextToken();
                                } while (token.get() != NULL && token->type() == TokenTypes::Transition);
                            }
                            
                            // parse the final markings
                            if (token.get() != NULL) {
                                expect(TokenTypes::FinalMarking, token.get());
                                do {
                                    PlaceMarking finalMarking = parseMarking(*net);
                                    net->addFinalPlaceMarking(finalMarking);
                                    token = m_tokenizer.nextToken();
                                } while (token.get() != NULL && token->type() == TokenTypes::FinalMarking);
                            }
                        }
                    }
                }
            }
            
            return net;
        }
    }
}
*/