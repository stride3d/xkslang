//
// Copyright (C) 

#ifndef XKSLANG_TESTS_UTILS_H
#define XKSLANG_TESTS_UTILS_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <tuple>
#include <string>
#include <vector>

#include "define.h"

#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

namespace xkslangtest
{

class Utils
{
public:
    //string utils
    static std::string trim(const std::string& str);
    static std::string trim(const std::string& str, char c);
    static std::string trim(const std::string& str, const std::string& chars);
    static std::string trimStart(const std::string& str, const std::string& chars);
    static std::string trimEnd(const std::string& str, const std::string& chars);
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    static bool startWith(const std::string& str, const std::string& prefix);
    static bool endWith(const std::string& str, const std::string& suffix);

    //Files IO
    // Reads the content of the file at the given |path|. On success, returns true
    // and the contents; otherwise, returns false and an empty string.
    static bool ReadFile(const std::string& path, std::string& output);
    static std::pair<bool, std::vector<uint32_t> > ReadSpvBinaryFile(const std::string& path);

    // Writes the given |contents| into the file at the given |path|. Returns true on successful output.
    static bool WriteFile(const std::string& path, const std::string& contents);

    // Returns the suffix of the given |name|.
    static std::string GetSuffix(const std::string& name);
    static std::string RemoveSuffix(const std::string& name);
    static std::string GetDirectoryFromFileFullName(const std::string& fullName);
    //========================================================================================
};

}  // namespace glslangtest

#endif  // XKSLANG_TESTS_UTILS_H
