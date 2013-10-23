//
//  LolaNetParser.h
//  Tippi
//
//  Created by Kristian Duske on 29.08.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__LolaNetParser__
#define __Tippi__LolaNetParser__

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Tippi {
    class Place;
    class PlaceMarking;
    class Transition;
    class Net;

    namespace Lola {
        typedef unsigned int TokenType;
        namespace TokenTypes {
            static const TokenType Comma        = 1 <<  0; // 1
            static const TokenType Colon        = 1 <<  1; // 2
            static const TokenType Semicolon    = 1 <<  2; // 4
            static const TokenType OParen       = 1 <<  3; // 8
            static const TokenType CParen       = 1 <<  4; // 16
            static const TokenType Net          = 1 <<  5; // 32
            static const TokenType TimeNet      = 1 <<  6; // 64
            static const TokenType Place        = 1 <<  7; // 128
            static const TokenType Safe         = 1 <<  8; // 256
            static const TokenType InputPlaces  = 1 <<  9; // 512
            static const TokenType OutputPlaces = 1 << 10; // 1024
            static const TokenType Marking      = 1 << 11; // 2048
            static const TokenType Transition   = 1 << 12; // 4096
            static const TokenType Consume      = 1 << 13; // 8192
            static const TokenType Produce      = 1 << 14; // 16384
            static const TokenType Time         = 1 << 15; // 32768
            static const TokenType FinalMarking = 1 << 16; // 65536
            static const TokenType Number       = 1 << 17; // 131072
            static const TokenType Identifier   = 1 << 18; // 262144
            static const TokenType Infinity     = 1 << 19;
        };
        
        class Token {
        public:
            typedef std::auto_ptr<Token> Ptr;
        private:
            TokenType m_type;
            size_t m_line;
            size_t m_position;
            std::string m_data;
        public:
            Token(TokenType type, size_t line, size_t position, const std::string& data) :
            m_type(type),
            m_line(line),
            m_position(position),
            m_data(data) {}
            
            Token(TokenType type, size_t line, size_t position) :
            m_type(type),
            m_line(line),
            m_position(position) {}
            
            inline TokenType type() const {
                return m_type;
            }
            
            inline size_t line() const {
                return m_line;
            }
        
            inline size_t position() const {
                return m_position;
            }
        
            inline const std::string& data() const {
                return m_data;
            }
            
            inline const long number() const {
                return atoi(m_data.c_str());
            }
        };
        
        class Tokenizer {
        private:
            std::istream& m_stream;
            std::stringstream m_buffer;
            size_t m_line;
            size_t m_position;
            size_t m_length;
            std::vector<Token*> m_tokenStack;
            
            inline bool eof() const {
                return m_position >= m_length;
            }
            
            inline char nextChar() {
                if (eof())
                    return 0;
                
                char c;
                m_stream.get(c);
                m_position++;
                
                if (c == '\n')
                    m_line++;
                
                return c;
            }
            
            inline void putChar(char c) {
                m_stream.seekg(-1, std::ios::cur);
                m_position--;
                if (m_stream.eof())
                    m_stream.clear();
                
                if (c == '\n')
                    m_line--;
            }
            
            inline char peekChar() {
                if (eof())
                    return 0;
                
                return m_stream.peek();
            }
            
            Token::Ptr token(const std::string& data, bool number);
            Token::Ptr token(TokenType type);
        public:
            Tokenizer(std::istream& stream);
            ~Tokenizer();
            
            Token::Ptr nextToken();
            Token::Ptr peekToken();
            void pushToken(Token* token);
        };
        
        class ParserException : public std::exception {
        private:
            std::string m_message;
        public:
            ParserException(size_t line, const std::string& error) throw() {
                std::stringstream buffer;
                buffer << error;
                buffer << " at line " << line;
                m_message = buffer.str();
            }
                
                virtual ~ParserException() throw() {}
            
            virtual const char* what() const throw() {
                return m_message.c_str();
            }
        };
        
        class Parser {
        private:
            Tokenizer m_tokenizer;
            
            std::string tokenNames(TokenType types);
            void expect(TokenType expected, Token* actual);
            
            void parsePlaces(Net& net);
            void parseIOPlaces(Net& net, bool input);
            PlaceMarking parseMarking(Net& net);
            void parseTransition(Net& net);
            void parseArc(Net& net, Transition& transition, bool incoming);
        public:
            Parser(std::istream& stream);
            
            Net* parseTimeNet();
        };
    }
}


#endif /* defined(__Tippi__LolaNetParser__) */
