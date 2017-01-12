//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

#include "Xkslang.h"
#include "XkslMixer.h"
#include "SpirxStreamParser.h"

using namespace std;
using namespace xkslang;

static void error(vector<string>& msgs, string msg)
{
    msgs.push_back(string("Error: ") + msg);
}

static void warning(vector<string>& msgs, string msg)
{
    msgs.push_back(string("Warning: ") + msg);
}

XkslMixer::XkslMixer()
{
    listParsedSprx.clear();
}

XkslMixer::~XkslMixer()
{
    for (int i = 0; i < listParsedSprx.size(); ++i)
    {
        delete listParsedSprx[i];
    }
}

void XkslMixer::AddMixin(SPXBytecode* spirXBytecode)
{
    listMixins.push_back(spirXBytecode);
}

bool XkslMixer::GenerateBytecode(SPVBytecode& bytecode, ShadingStage stage, string entryPoint, vector<string>& msgs)
{
    if (listMixins.size() == 0)
    {
        error(msgs, "No bytecodes in the mixin list");
        return false;
    }

    //======================================================================================
    // Parse the SPIRX bytecodes (disassemble the bytecode instructions)
    for (int i=0; i<listMixins.size(); ++i)
    {
        SPXBytecode* spirXBytecode = listMixins[i];

        SpirxStreamParser* sprxStream = ParseSPXBytecode(spirXBytecode, msgs);
        if (sprxStream == nullptr) {
            error(msgs, "Fail to parse the list of mixins");
            return false;
        }
        listParsedSprx.push_back(sprxStream);

        //process the bytecode
        bool success = sprxStream->ProcessSpriXBytecode();
        if (!success)
        {
            error(msgs, "Fail to process the bytecode");
            return false;
        }
    }

    return true;
}

SpirxStreamParser* XkslMixer::ParseSPXBytecode(SPXBytecode* spirXBytecode, std::vector<std::string>& msgs)
{
    const vector<uint32_t>& stream = spirXBytecode->getBytecodeStream();

    SpirxStreamParser* streamParser = new SpirxStreamParser(stream);

    //=============================================================================
    //Validate SPIRX header
    uint32_t magicNumber, moduleVersion, generatorMagicNumber;
    if (!streamParser->ValidateHeader(magicNumber, moduleVersion, generatorMagicNumber))
    {
        streamParser->copyMessagesTo(msgs);
        delete streamParser; return nullptr;
    }

    // Check Magic number
    if (magicNumber != Xkslang::MagicNumber){
        error(msgs, "Bad magic number");
        delete streamParser; return nullptr;
    }

    //=============================================================================
    //Iterate through SPIRX instructions

    bool success = streamParser->DisassembleSpirXStream();
    if (!success)
    {
        streamParser->copyMessagesTo(msgs);
        delete streamParser; return nullptr;
    }

    return streamParser;

    /*
    SPVFunction parsedFunction;
    parsedFunction.byteCode = spirXBytecode;
    while (word < size)
    {
        int instructionStart = word;

        // Instruction wordCount and opcode
        uint32_t firstWord = stream[word];
        uint32_t wordCount = firstWord >> spv::WordCountShift;
        spv::Op opCode = (spv::Op)(firstWord & spv::OpCodeMask);
        uint32_t nextInst = word + wordCount;
        ++word;

        // Presence of full instruction
        if (nextInst > size)
            return error(msgs, "stream instruction terminated too early");

        // Base for computing number of operands; will be updated as more is learned
        unsigned numOperands = wordCount - 1;

        spv::Id typeId = 0;
        spv::Id resultId = 0;

        switch (opCode)
        {
            case spv::OpDecorate:
                //if (!DisassembleOpDecorateInstruction(stream, word, opCode, numOperands, typeId, resultId))
                    return error(msgs, "Failed to disassemble the instruction");
                break;

            case spv::OpFunction:
                parsedFunction.opStart = instructionStart;
                break;

            case spv::OpFunctionEnd:
                parsedFunction.opEnd = instructionStart;
                this->listFunctions.push_back(parsedFunction);
        
            default:
                break;
        }

        word = nextInst;
    }

    return true;*/
}
