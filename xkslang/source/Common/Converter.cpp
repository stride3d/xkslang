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

bool Converter::ConvertSpvToAsciiText(const vector<uint32_t>& bytecode, string& text)
{
    if (bytecode.size() == 0) return true;

    ostringstream disassembly_stream;
    spv::Parameterize();
    spv::Disassemble(disassembly_stream, bytecode);
    text = disassembly_stream.str();
    return true;
}

bool Converter::ConvertSpvToHLSL(const vector<uint32_t>& bytecode, string& hlslShader)
{
    int argc;
    char **argv;
    
    string outputFile;
    string spvFile;

    {
        argc = 7;
        const char *args[] = {
            "spirv_cross.lib",
            "--hlsl",
            "--shader-model",
            "40",
            "--output",
            outputFile.c_str(),
            spvFile.c_str()
        };
        argv = (char**)args;
    }

    return spirv_cross::SPIRV_CROSS::executeCmd(argc, argv);

    return true;
}