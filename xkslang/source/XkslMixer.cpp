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

#include "XkslMixer.h"
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

bool XkslMixer::GetCurrentMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    spxStreamRemapper->GetMixinBytecode(output.getWritableBytecodeStream());

    return true;
}

bool XkslMixer::Compile(vector<XkslMixerOutputStage>& outputStages, SpvBytecode& compiledSpv, std::vector<std::string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    if (outputStages.size() == 0)
        return error(messages, "you must define some stage outputs");

    //=============================================================================================================================================
    //=============================================================================================================================================
    //We clone the stream before compiling it: we want to keep the original stream as it is, so that user can keep mixin and updating it if need
    SpxStreamRemapper* clonedSpxStream = spxStreamRemapper->Clone();
    if (clonedSpxStream == nullptr)
        return error(messages, "Failed to clone the SpxStreamRemapper");

    //TODO
    /*if (!clonedSpxStream->Compile())
    {
        clonedSpxStream->copyMessagesTo(messages);
        return error(messages, "Fail to compile the mixin");
    }*/

    if (!clonedSpxStream->FinalizeMixin()){
        clonedSpxStream->copyMessagesTo(messages);
        return error(messages, "Fail to finalize the mixin");
    }

    if (!clonedSpxStream->GetMixinBytecode(compiledSpv.getWritableBytecodeStream())){
        clonedSpxStream->copyMessagesTo(messages);
        return error(messages, "Fail to get the mixin bytecode");
    }
        
    for (int i=0; i<outputStages.size(); ++i)
    {
        XkslMixerOutputStage& outputStage = outputStages[i];
        bool success = clonedSpxStream->GenerateSpvStageBytecode(outputStage.stage, outputStage.entryPoint, outputStage.resultingBytecode);
        if (!success)
        {
            clonedSpxStream->copyMessagesTo(messages);
            return error(messages, string("Fail to generate SPV stage bytecode for stage=\"") + GetShadingStageLabel(outputStage.stage) + string("\""));
        }
    }

    delete clonedSpxStream;
    //=============================================================================================================================================
    //=============================================================================================================================================

    return true;
}
