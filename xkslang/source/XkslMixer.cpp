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

void XkslMixer::StartMixin()
{
    SpxStreamRemapper::ResetMergeOperationId();
}

void XkslMixer::ReleaseMixin()
{

}

XkslMixer::XkslMixer()
{
    spxStreamRemapper = nullptr;
    //listSpxStream.clear();
}

XkslMixer::~XkslMixer()
{
    if (spxStreamRemapper != nullptr) delete spxStreamRemapper;
}

bool XkslMixer::GetListAllShadersFromBytecode(SpxBytecode& spxBytecode, vector<string>& vecShaderName, vector<string>& msgs)
{
    vecShaderName.clear();
    SpxStreamRemapper bytecodeStream;
    if (!bytecodeStream.SetBytecode(spxBytecode)) {
        bytecodeStream.copyMessagesTo(msgs);
        return false;
    }

    vector<SpxStreamRemapper::ParsedObjectData> listParsedObjectsData;
    bool res = bytecodeStream.BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        msgs.push_back("Failed to build the bytecode declaration map");
        bytecodeStream.copyMessagesTo(msgs);
        return false;
    }

    int countParsedObjects = listParsedObjectsData.size();
    for (int i = 0; i < countParsedObjects; ++i)
    {
        SpxStreamRemapper::ParsedObjectData& parsedData = listParsedObjectsData[i];
        if (parsedData.kind == SpxStreamRemapper::ObjectInstructionTypeEnum::Shader)
        {
            string declarationName;
            bool hasDeclarationName = bytecodeStream.GetDeclarationNameForId(parsedData.resultId, declarationName);
            if (!hasDeclarationName) {
                msgs.push_back(string("The XkslShader has no declaration name. Id: ") + to_string(parsedData.resultId));
                return false;
            }
            vecShaderName.push_back(declarationName);
        }
    }

    return true;
}

bool XkslMixer::Mixin(const SpxBytecode& spirXBytecode, vector<string>& msgs)
{
    //listMixins.push_back(spirXBytecode);

    if (spxStreamRemapper == nullptr) spxStreamRemapper = new SpxStreamRemapper();

    return false;

    //if (!spxStreamRemapper->MixWithBytecode(spirXBytecode))
    //{
    //    spxStreamRemapper->copyMessagesTo(msgs);
    //    return error(msgs, string("Fail to mix the bytecode:" + spirXBytecode.GetName()) );
    //}
    //return true;
}

bool XkslMixer::Mixin(const SpxBytecode& spirXBytecode, const string& shaderName, vector<string>& msgs)
{
    vector<string> shaders;
    shaders.push_back(shaderName);
    return Mixin(spirXBytecode, shaders, msgs);
}

bool XkslMixer::Mixin(const SpxBytecode& spirXBytecode, const vector<string>& shaders, vector<string>& msgs)
{
    if (spxStreamRemapper == nullptr) spxStreamRemapper = new SpxStreamRemapper();

    if (!spxStreamRemapper->MixWithShadersFromBytecode(spirXBytecode, shaders))
    {
        spxStreamRemapper->copyMessagesTo(msgs);
        return error(msgs, string("Fail to mix the shaders from bytecode: " + spirXBytecode.GetName()));
    }

    return true;
}

bool XkslMixer::AddComposition(const string& shaderName, const string& variableName, XkslMixer* mixerSource, vector<string>& msgs)
{
    if (spxStreamRemapper == nullptr) {
        return error(msgs, "mixer is empty");
    }
    if (mixerSource == nullptr || mixerSource->spxStreamRemapper == nullptr) {
        return error(msgs, "mixerSource is null");
    }
    if (shaderName.size() == 0) {
        return error(msgs, "shaderName is invalid");
    }
    if (variableName.size() == 0) {
        return error(msgs, "variableName is invalid");
    }

    if (!spxStreamRemapper->AddComposition(shaderName, variableName, mixerSource->spxStreamRemapper, msgs))
    {
        spxStreamRemapper->copyMessagesTo(msgs);
        return error(msgs, "Failed to add the composition to the mixer");
    }
    return true;
}

bool XkslMixer::GetCurrentMixinBytecode(SpxBytecode& output, vector<string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    spxStreamRemapper->GetMixinBytecode(output.getWritableBytecodeStream());

    return true;
}

bool XkslMixer::Compile(vector<OutputStageBytecode>& outputStages, vector<string>& messages, SpvBytecode* compiledSpv, SpvBytecode* finalSpv, SpvBytecode* errorLatestSpv)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    if (outputStages.size() == 0)
        return error(messages, "no output stages defined");

    vector<XkslMixerOutputStage> vecMixerOutputStages;
    for (unsigned int i=0; i < outputStages.size(); ++i)
    {
        OutputStageBytecode* pOutputStage = &(outputStages[i]);
        XkslMixerOutputStage xkslOutputStage(pOutputStage);
        vecMixerOutputStages.push_back(xkslOutputStage);
    }

    //=============================================================================================================================================
    //=============================================================================================================================================
    //We clone the stream before compiling it: we want to keep the original stream as it is, so that user can keep mixin and updating it if need
    SpxStreamRemapper* clonedSpxStream = spxStreamRemapper->Clone();
    if (clonedSpxStream == nullptr) return error(messages, "Failed to clone the SpxStreamRemapper");

    //===================================================================================================================
    //===================================================================================================================
    // apply all composition instances
    if (!clonedSpxStream->ApplyCompositionInstancesToBytecode())
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Failed to apply all compositions to the bytecode");
    }

    if (compiledSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(compiledSpv->getWritableBytecodeStream());

    //===================================================================================================================
    //===================================================================================================================
    // remove unused shaders
    if (!clonedSpxStream->RemoveAllUnusedShaders(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to remova all unused shaders");
    }

    //===================================================================================================================
    //===================================================================================================================
    if (!clonedSpxStream->CompileMixinForStages(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to finalize the mixin");
    }

    if (finalSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(finalSpv->getWritableBytecodeStream());
    
    delete clonedSpxStream;
    //=============================================================================================================================================
    //=============================================================================================================================================

    return true;
}
