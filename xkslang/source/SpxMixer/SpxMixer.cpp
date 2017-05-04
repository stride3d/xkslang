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

#include "SpxMixer.h"
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

void SpxMixer::StartMixin()
{
    SpxStreamRemapper::ResetMergeOperationId();
}

void SpxMixer::ReleaseMixin()
{

}

SpxMixer::SpxMixer()
{
    spxStreamRemapper = nullptr;
    //listSpxStream.clear();
}

SpxMixer::~SpxMixer()
{
    if (spxStreamRemapper != nullptr) delete spxStreamRemapper;
}

bool SpxMixer::GetListAllShadersFromBytecode(SpxBytecode& spxBytecode, vector<string>& vecShaderName, vector<string>& msgs)
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

bool SpxMixer::Mixin(const SpxBytecode& spirXBytecode, vector<string>& msgs)
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

bool SpxMixer::Mixin(const SpxBytecode& spirXBytecode, const string& shaderName, vector<string>& msgs)
{
    vector<string> shaders;
    shaders.push_back(shaderName);
    return Mixin(spirXBytecode, shaders, msgs);
}

bool SpxMixer::Mixin(const SpxBytecode& spirXBytecode, const vector<string>& shaders, vector<string>& msgs)
{
    if (spxStreamRemapper == nullptr) spxStreamRemapper = new SpxStreamRemapper();

    if (!spxStreamRemapper->MixWithShadersFromBytecode(spirXBytecode, shaders))
    {
        spxStreamRemapper->copyMessagesTo(msgs);
        return error(msgs, string("Fail to mix the shaders from bytecode: " + spirXBytecode.GetName()));
    }

    return true;
}

bool SpxMixer::AddComposition(const string& shaderName, const string& variableName, SpxMixer* mixerSource, vector<string>& msgs)
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

bool SpxMixer::GetCurrentMixinBytecode(SpxBytecode& output, vector<string>& messages)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    spxStreamRemapper->GetMixinBytecode(output.getWritableBytecodeStream());

    return true;
}

bool SpxMixer::Compile(vector<OutputStageBytecode>& outputStages, vector<string>& messages,
    SpvBytecode* composedSpv, SpvBytecode* streamsMergeSpv, SpvBytecode* streamsReshuffledSpv, SpvBytecode* mergedCBuffersSpv, SpvBytecode* finalSpv, SpvBytecode* errorLatestSpv)
{
    if (spxStreamRemapper == nullptr)
        return error(messages, "you must process some mixin first");

    if (outputStages.size() == 0)
        return error(messages, "no output stages defined");

    //===================================================================================================================
    //put the output stages in the correct order
    vector<ShadingStageEnum> stagePipeline;
    vector<XkslMixerOutputStage> vecMixerOutputStages;
    SpxStreamRemapper::GetStagesPipeline(stagePipeline);
    for (unsigned int iStage=0; iStage < stagePipeline.size(); ++iStage)
    {
        ShadingStageEnum stage = stagePipeline[iStage];
        for (unsigned int io = 0; io < outputStages.size(); ++io)
        {
            bool stageFound = false;
            if (outputStages[io].stage == stage)
            {
                if (stageFound) return error(messages, string("2 output stages found for stage: ") + GetShadingStageLabel(stage));

                OutputStageBytecode* pOutputStage = &(outputStages[io]);
                XkslMixerOutputStage xkslOutputStage(pOutputStage);
                vecMixerOutputStages.push_back(xkslOutputStage);
                stageFound = true;
            }
        }
    }

    //We clone the stream before compiling it: we want to keep the original stream as it is, so that user can keep mixin and updating it if need
    SpxStreamRemapper* clonedSpxStream = spxStreamRemapper->Clone();
    if (clonedSpxStream == nullptr) return error(messages, "Failed to clone the SpxStreamRemapper");

    //===================================================================================================================
    //===================================================================================================================
    // initialize the compilation process (validate the bytecode, search for the output stage entry point functions, ...)
    if (!clonedSpxStream->InitializeCompilationProcess(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Failed to initialize the compilation process");
    }

    //===================================================================================================================
    // Process compositions
    //===================================================================================================================
    // apply all composition instances
    if (!clonedSpxStream->ApplyCompositionInstancesToBytecode())
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Failed to apply all compositions to the bytecode");
    }

    if (composedSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(composedSpv->getWritableBytecodeStream());

    //===================================================================================================================
    // Process streams
    //===================================================================================================================
    // merge all stream variables into a global single struct
    SpxStreamRemapper::TypeStructMemberArray globalListOfMergedStreamVariables;
    if (!clonedSpxStream->MergeStreamMembers(globalListOfMergedStreamVariables))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to merge the streams");
    }

    if (streamsMergeSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(streamsMergeSpv->getWritableBytecodeStream());

    //===================================================================================================================
    // analyse the stream and cbuffers usage for each stage
    if (!clonedSpxStream->AnalyseStreamsAndCBuffersAccessesForOutputStages(vecMixerOutputStages, globalListOfMergedStreamVariables))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Analysing the streams failed");
    }

    if (vecMixerOutputStages.size() == 1 && (vecMixerOutputStages[0].outputStage->stage == ShadingStageEnum::Vertex || vecMixerOutputStages[0].outputStage->stage == ShadingStageEnum::Pixel))
    {
        //this would normally returns an error in the streams validation, but we just skip this step for now to allow compilation of partial sample effects
    }
    else
    {
        //validate conformity with stream flow
        if (!clonedSpxStream->ValidateStagesStreamMembersFlow(vecMixerOutputStages, globalListOfMergedStreamVariables))
        {
            clonedSpxStream->copyMessagesTo(messages);
            if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
            delete clonedSpxStream;
            return error(messages, "Validating the stages stream members flow failed");
        }

        //===================================================================================================================
        //===================================================================================================================
        // reshuffle the stream members
        if (!clonedSpxStream->ReshuffleStreamVariables(vecMixerOutputStages, globalListOfMergedStreamVariables))
        {
            clonedSpxStream->copyMessagesTo(messages);
            if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
            delete clonedSpxStream;
            return error(messages, "Fail to reshuffle the streams");
        }

        if (streamsReshuffledSpv != nullptr)
            clonedSpxStream->GetMixinBytecode(streamsReshuffledSpv->getWritableBytecodeStream());
    }

    //===================================================================================================================
    // Process cbuffers
    //===================================================================================================================
    // remove unused cbuffers, merge used cbuffers havind same name
    vector<EffectReflection::ConstantBuffer> listConstantBuffers;
    if (!clonedSpxStream->ProcessCBuffers(vecMixerOutputStages, listConstantBuffers))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to process the cbuffers");
    }
    if (mergedCBuffersSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(mergedCBuffersSpv->getWritableBytecodeStream());

    //===================================================================================================================
    //===================================================================================================================
    // remove unused shaders (shader whose methods or members are never called by the output stages)
    if (!clonedSpxStream->RemoveAllUnusedShaders(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to remove all unused shaders");
    }

    //===================================================================================================================
    // Convert SPX to SPV
    //===================================================================================================================
    if (!clonedSpxStream->RemoveAndConvertSPXExtensions())
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to remove and convert spx extensions");
    }

#ifdef XKSLANG_DEBUG_MODE
    //Before final compilation step: do a full sanity check on the bytecode (useful in debug to help detecting problems earlier)
    if (!clonedSpxStream->ProcessBytecodeAndDataSanityCheck())
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to process a bytecode and data sanity check");
    }
#endif

    if (finalSpv != nullptr)
        clonedSpxStream->GetMixinBytecode(finalSpv->getWritableBytecodeStream());

    //===================================================================================================================
    // Build SPV bytecode for each output stages
    //===================================================================================================================
    if (!clonedSpxStream->GenerateBytecodeForAllStages(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to finalize the mixin");
    }

    delete clonedSpxStream;
    //=============================================================================================================================================
    //=============================================================================================================================================

    return true;
}
