//
// Copyright (C)

#include "xkslangUtils.h"

using namespace xkslang;
using namespace std;

//========================================================================================
//========================================================================================
//string utils
bool XkslangUtils::startWith(const string& str, const string& prefix)
{
    unsigned int prefixSize = (int)prefix.size();
    if (prefixSize == 0) return false;

    return (str.compare(0, prefixSize, prefix) == 0);
}

bool XkslangUtils::endWith(const string& str, const string& suffix)
{
    int suffixSize = (int)suffix.size();
    int startPos = (int)str.size() - suffixSize;
    if (startPos < 0 || suffixSize == 0) return false;

    return (str.compare(startPos, suffixSize, suffix) == 0);
}

string XkslangUtils::trim(const string& str)
{
    return trim(str, ' ');
}

string XkslangUtils::trim(const string& str, char c)
{
    size_t first = str.find_first_not_of(c);
    if (first == string::npos) return str;
    size_t last = str.find_last_not_of(c);
    return str.substr(first, (last - first + 1));
}

string XkslangUtils::trimStart(const string& str, const string& chars)
{
    unsigned int len = str.size();
    unsigned int countChars = chars.size();
    char c;
    int startPos = 0;
    bool loop = true;
    while (loop)
    {
        if (startPos == len) return "";

        loop = false;
        c = str[startPos];
        for (unsigned int d = 0; d < countChars; d++)
        {
            if (c == chars[d]) {
                startPos++;
                loop = true;
                break;
            }
        }
    }

    return str.substr(startPos);
}

string XkslangUtils::trimEnd(const string& str, const string& chars)
{
    unsigned int len = str.size();
    unsigned int countChars = chars.size();
    char c;
    int endPos = len - 1;
    bool loop = true;
    while (loop)
    {
        if (endPos < 0) return "";

        loop = false;
        c = str[endPos];
        for (unsigned int d = 0; d < countChars; d++)
        {
            if (c == chars[d]) {
                endPos--;
                loop = true;
                break;
            }
        }
    }

    return str.substr(0, (endPos + 1));
}

string XkslangUtils::trim(const string& str, const string& chars)
{
    unsigned int len = str.size();
    unsigned int countChars = chars.size();
    char c;
    int startPos = 0;
    bool loop = true;
    while (loop)
    {
        if (startPos == len) return "";

        loop = false;
        c = str[startPos];
        for (unsigned int d = 0; d < countChars; d++)
        {
            if (c == chars[d]) {
                startPos++;
                loop = true;
                break;
            }
        }
    }

    int endPos = len - 1;
    loop = true;
    while (loop)
    {
        if (endPos < startPos) return "";

        loop = false;
        c = str[endPos];
        for (unsigned int d = 0; d < countChars; d++)
        {
            if (c == chars[d]) {
                endPos--;
                loop = true;
                break;
            }
        }
    }

    return str.substr(startPos, (endPos - startPos + 1));
}

void XkslangUtils::replaceAll(string& str, const string& from, const string& to)
{
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
