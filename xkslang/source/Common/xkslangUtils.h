//
// Copyright (C) 

#ifndef XKSLANG_XKSLANG_UTILS_H
#define XKSLANG_XKSLANG_UTILS_H

#include <string>

namespace xkslang
{
    
class XkslangUtils
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
    static bool getFunctionParameterString(const std::string& instruction, std::string& parameterStr);
};

} //namespace xkslang

#endif  // XKSLANG_XKSLANG_UTILS_H
