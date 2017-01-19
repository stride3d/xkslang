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
//files IO
bool Utils::ReadFile(const string& path, string& output)
{
    ifstream fstream(path, ios::in);
    if (fstream) {
        string contents;
        fstream.seekg(0, ios::end);
        contents.reserve((string::size_type)fstream.tellg());
        fstream.seekg(0, ios::beg);
        contents.assign((istreambuf_iterator<char>(fstream)),
            istreambuf_iterator<char>());

        output = contents;
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
    if (!fstream) return false;
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

//========================================================================================
//========================================================================================
