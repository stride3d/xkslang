//
// Copyright (C) 

#ifndef XKSLANG_CONVERTER_H__
#define XKSLANG_CONVERTER_H__

#include <vector>
#include <string>

#include "define.h"

namespace xkslang
{

class Converter
{
public:
    static bool ConvertBytecodeToAscii(const std::vector<uint32_t>& bytecode, std::string& text);
};

}  // namespace xkslang

#endif  // XKSLANG_CONVERTER_H__
