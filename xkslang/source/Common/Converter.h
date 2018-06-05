//
// Copyright (C) 

#ifndef XKSLANG_CONVERTER_H__
#define XKSLANG_CONVERTER_H__

#include <vector>
#include <string>

#include "xkslangDefine.h"

namespace xkslang
{

class Converter
{
public:
    static bool ConvertBytecodeToAsciiText(const std::vector<uint32_t>& bytecode, std::string& text);

    static bool ConvertBytecodeToHlsl(const std::vector<uint32_t>& bytecode, int shaderModel, std::string& hlslShader);

    static bool ConvertBytecodeToGlsl(const std::vector<uint32_t>& bytecode, bool es, int version, std::string& glslShader);
};

}  // namespace xkslang

#endif  // XKSLANG_CONVERTER_H__
