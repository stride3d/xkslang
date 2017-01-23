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
#include "SpxStreamParser.h"
#include "SpxMixerToSpvBuilder.h"
#include "SpxStreamRemapper.h"

using namespace std;
using namespace xkslang;

static bool error(vector<string>& msgs, string msg)
{
    msgs.push_back(string("Error: ") + msg);
    return false;
}

static void warning(vector<string>& msgs, string msg)
{
    msgs.push_back(string("Warning: ") + msg);
}
//=============================================================================================================//
//=============================================================================================================//

XkslMixer::XkslMixer()
{
    spxStreamRemapper = nullptr;
    //listSpxStream.clear();
}

XkslMixer::~XkslMixer()
{
    if (spxStreamRemapper != nullptr) delete spxStreamRemapper;
}

bool XkslMixer::Mixin(const SpxBytecode& spirXBytecode, std::vector<std::string>& msgs)
{
    //listMixins.push_back(spirXBytecode);

    if (spxStreamRemapper == nullptr) spxStreamRemapper = new SpxStreamRemapper();

    if (!spxStreamRemapper->MixWithSpxBytecode(spirXBytecode))
    {
        spxStreamRemapper->copyMessagesTo(msgs);
        return error(msgs, string("Fail to mix the bytecode:" + spirXBytecode.GetName()) );
    }

    return true;
}

bool XkslMixer::FinalizeMixin(std::vector<std::string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "The mixin AST must been created first");

    if (!spxStreamRemapper->FinalizeMixin())
    {
        spxStreamRemapper->copyMessagesTo(messages);
        return error(messages, "Fail to finalize the mixin");
    }

    return true;
}

bool XkslMixer::GetMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "The mixin AST must been created first");

    spxStreamRemapper->GetMappedSpxBytecode(output);

    return true;
}

bool XkslMixer::GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "The mixin AST must been created first");

    bool success = spxStreamRemapper->GenerateSpvStageBytecode(stage, entryPoint, output);
    if (!success)
    {
        spxStreamRemapper->copyMessagesTo(messages);
        return false;
    }

    return true;
}
