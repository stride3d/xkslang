//
// Copyright (C)

#include <sstream>

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

#include "Converter.h"

#include "../source/SPIRV-Cross/spirv_cross.hpp"

using namespace std;
using namespace xkslang;

bool Converter::ConvertBytecodeToAsciiText(const vector<uint32_t>& bytecode, string& text)
{
    if (bytecode.size() == 0) return true;

    ostringstream disassembly_stream;
    spv::Parameterize();
    spv::Disassemble(disassembly_stream, bytecode);
    text = disassembly_stream.str();
    return true;
}

bool Converter::ConvertBytecodeToHlsl(const vector<uint32_t>& bytecode, int shaderModel, string& hlslShader)
{
    int res = spirv_cross::SPIRV_CROSS::convertSpvBytecodeToHlsl(bytecode, shaderModel, hlslShader);
    return (res == 0);
}

bool Converter::ConvertBytecodeToGlsl(const vector<uint32_t>& bytecode, string& glslShader)
{
    int res = spirv_cross::SPIRV_CROSS::convertSpvBytecodeToGlsl(bytecode, glslShader);
    return (res == 0);
}