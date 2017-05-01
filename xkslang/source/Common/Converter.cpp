//
// Copyright (C)

#include <sstream>

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

#include "Converter.h"

using namespace std;
using namespace xkslang;

bool Converter::ConvertBytecodeToAscii(const vector<uint32_t>& bytecode, string& text)
{
    if (bytecode.size() == 0) return true;

    ostringstream disassembly_stream;
    spv::Parameterize();
    spv::Disassemble(disassembly_stream, bytecode);
    text = disassembly_stream.str();
    return true;
}