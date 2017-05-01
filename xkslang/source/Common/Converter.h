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
    static bool ConvertSpvToAsciiText(const std::vector<uint32_t>& bytecode, std::string& text);

    static bool ConvertSpvToHLSL(const std::vector<uint32_t>& bytecode, std::string& hlslShader);
};

}  // namespace xkslang

#endif  // XKSLANG_CONVERTER_H__
