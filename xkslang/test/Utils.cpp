//
// Copyright (C) 

#include <cassert>

#include "Utils.h"

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

using namespace xkslangtest;
using namespace std;

//========================================================================================
//========================================================================================
//string utils
bool Utils::startWith(const string& str, const string& prefix)
{
    unsigned int prefixSize = (int)prefix.size();
    if (prefixSize == 0) return false;

    return (str.compare(0, prefixSize, prefix) == 0);
}

bool Utils::endWith(const string& str, const string& suffix)
{
    int suffixSize = (int)suffix.size();
    int startPos = (int)str.size() - suffixSize;
    if (startPos < 0 || suffixSize == 0) return false;

    return (str.compare(startPos, suffixSize, suffix) == 0);
}

string Utils::trim(const string& str)
{
    return trim(str, ' ');
}

string Utils::trim(const string& str, char c)
{
    size_t first = str.find_first_not_of(c);
    if (first == string::npos) return str;
    size_t last = str.find_last_not_of(c);
    return str.substr(first, (last - first + 1));
}

string Utils::trimStart(const string& str, const string& chars)
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

string Utils::trimEnd(const string& str, const string& chars)
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

string Utils::trim(const string& str, const string& chars)
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
            if (c == chars[d]){
                startPos++;
                loop = true;
                break;
            }
        }
    }

    int endPos =  len - 1;
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

void Utils::replaceAll(string& str, const string& from, const string& to)
{
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

//========================================================================================
//========================================================================================
//files IO
bool Utils::ReadFile(const string& path, string& fileContent)
{
    ifstream fstream(path, ios::in);
    if (fstream) {
        string contents;
        fstream.seekg(0, ios::end);
        contents.reserve((string::size_type)fstream.tellg());
        fstream.seekg(0, ios::beg);
        contents.assign((istreambuf_iterator<char>(fstream)),
            istreambuf_iterator<char>());

        fileContent = contents;
        return true;
    }

    return false;
}

pair<bool, vector<uint32_t> > Utils::ReadSpvBinaryFile(const string& path)
{
    ifstream fstream(path, fstream::in | fstream::binary);

    if (!fstream)
        return make_pair(false, vector<uint32_t>());

    vector<uint32_t> contents;

    // Reserve space (for efficiency, not for correctness)
    fstream.seekg(0, fstream.end);
    contents.reserve(size_t(fstream.tellg()) / sizeof(uint32_t));
    fstream.seekg(0, fstream.beg);

    // There is no istream iterator traversing by uint32_t, so we must loop.
    while (!fstream.eof()) {
        uint32_t inWord;
        fstream.read((char *)&inWord, sizeof(inWord));

        if (!fstream.eof())
            contents.push_back(inWord);
    }

    return make_pair(true, contents); // hopefully, c++11 move semantics optimizes the copy away.
}

bool Utils::WriteFile(const string& path, const string& contents)
{
    ofstream fstream(path, ios::out);
    if (!fstream){
        cout << "failed to write file: " << path;
        return false;
    }
    fstream << contents;
    fstream.flush();
    return true;
}

string Utils::GetSuffix(const string& name)
{
    const size_t pos = name.rfind('.');
    return (pos == string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

string Utils::RemoveSuffix(const string& name)
{
    const size_t pos = name.rfind('.');
    if (pos == string::npos) return name;
    return name.substr(0, pos);
}

string Utils::GetDirectoryFromFileFullName(const string& fullName)
{
    const size_t pos = fullName.find_last_of("\\/");
    if (pos == string::npos) return fullName;
    return fullName.substr(0, pos);
}

//========================================================================================
//========================================================================================
