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

#ifndef Tippi_StringUtils_h
#define Tippi_StringUtils_h

#include <cassert>
#include <map>
#include <sstream>
#include <string>
#include <vector>

typedef std::string String;
typedef std::stringstream StringStream;
typedef std::vector<String> StringList;
typedef std::map<String, String> StringMap;
static const StringList EmptyStringList;

namespace StringUtils {
    String formatString(const char* format, va_list arguments);
    String trim(const String& str, const String& chars = " \n\t\r");

    bool isPrefix(const String& str, const String& prefix);
    bool containsCaseSensitive(const String& haystack, const String& needle);
    bool containsCaseInsensitive(const String& haystack, const String& needle);
    void sortCaseSensitive(StringList& strs);
    void sortCaseInsensitive(StringList& strs);
    bool caseSensitiveEqual(const String& str1, const String& str2);
    bool caseInsensitiveEqual(const String& str1, const String& str2);
    long makeHash(const String& str);
    String toLower(const String& str);
    String replaceChars(const String& str, const String& needles, const String& replacements);
    String capitalize(const String& str);
    String escape(const String& str, const String& chars);
    String unescape(const String& str, const String& chars);

    template <typename D>
    StringList split(const String& str, D d) {
        if (str.empty())
            return EmptyStringList;
        
        const size_t first = str.find_first_not_of(d);
        if (first == String::npos)
            return EmptyStringList;
        const size_t last = str.find_last_not_of(d);
        assert(last != String::npos);
        assert(first <= last);
        
        StringList result;
        
        size_t lastPos = first;
        size_t pos = lastPos;
        while ((pos = str.find_first_of(d, pos)) < last) {
            result.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = ++pos;
        }
        if (lastPos <= last)
            result.push_back(str.substr(lastPos, last - lastPos + 1));
        return result;
    }
    
    template <typename D>
    String join(const StringList& strs, const D& d) {
        if (strs.empty())
            return "";
        if (strs.size() == 1)
            return strs[0];
        
        StringStream result;
        result << strs[0];
        for (size_t i = 1; i < strs.size(); i++)
            result << d << strs[i];
        return result.str();
    }
}

#endif
