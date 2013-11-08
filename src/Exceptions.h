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

#ifndef Tippi_Exceptions_h
#define Tippi_Exceptions_h

#include <exception>

#include "StringUtils.h"

namespace Tippi {
    
    class Exception : public std::exception {
    protected:
        String m_msg;
    public:
        Exception() throw() {}
        Exception(const String& str) throw() : m_msg(str) {}
        virtual ~Exception() throw() {}

        const char* what() const throw() {
            return m_msg.c_str();
        }
    };
    
    template <class C>
    class ExceptionStream : public Exception {
    public:
        ExceptionStream() throw() {}
        ExceptionStream(const String& str) throw() : Exception(str) {}
        virtual ~ExceptionStream() throw() {}
        
        template <typename T>
        C& operator<< (T value) {
            StringStream stream;
            stream << m_msg << value;
            m_msg = stream.str();
            return static_cast<C&>(*this);
        }
    };
    
    class NetException : public ExceptionStream<NetException> {
    public:
        NetException() throw() {}
        NetException(const String& str) throw() : ExceptionStream(str) {}
        ~NetException() throw() {}
    };
    
    class FiringRuleException : public ExceptionStream<FiringRuleException> {
    public:
        FiringRuleException() throw() {}
        FiringRuleException(const String& str) throw() : ExceptionStream(str) {}
        ~FiringRuleException() throw() {}
    };


    class AutomatonException : public ExceptionStream<NetException> {
    public:
        AutomatonException() throw() {}
        AutomatonException(const String& str) throw() : ExceptionStream(str) {}
        ~AutomatonException() throw() {}
    };
    
    class ParserException : public ExceptionStream<ParserException> {
    public:
        ParserException() throw() {}
        ParserException(const String& str) throw() : ExceptionStream(str) {}
        ParserException(const size_t line, const size_t column, const String& str = "") throw() : ExceptionStream() {
            *this << "Line " << line;
            *this << ", column " << column;
            *this << " - " << str;
        }
        ~ParserException() throw() {}
    };
}

#endif
