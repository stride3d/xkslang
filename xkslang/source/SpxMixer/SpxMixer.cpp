//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SpxMixer.h"
#include "SpxCompiler.h"

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

void SpxMixer::StartMixinEffect()
{
    SpxCompiler::ResetMergeOperationId();
}

void SpxMixer::StopMixinEffect()
{

}

SpxMixer::SpxMixer()
{
    spxCompiler = nullptr;
    //listSpxStream.clear();
}

SpxMixer::~SpxMixer()
{
    if (spxCompiler != nullptr) delete spxCompiler;
}

bool SpxMixer::GetListAllShadersFromBytecode(SpxBytecode& spxBytecode, vector<string>& vecShaderName, vector<string>& msgs)
{
    vecShaderName.clear();
    SpxCompiler bytecodeStream;
    if (!bytecodeStream.SetBytecode(spxBytecode)) {
        bytecodeStream.copyMessagesTo(msgs);
        return false;
    }

    vector<SpxCompiler::ParsedObjectData> listParsedObjectsData;
    bool res = bytecodeStream.BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        msgs.push_back("Failed to build the bytecode declaration map");
        bytecodeStream.copyMessagesTo(msgs);
        return false;
    }

    unsigned int countParsedObjects = (unsigned int)listParsedObjectsData.size();
    for (unsigned int i = 0; i < countParsedObjects; ++i)
    {
        SpxCompiler::ParsedObjectData& parsedData = listParsedObjectsData[i];
        if (parsedData.kind == SpxCompiler::ObjectInstructionTypeEnum::Shader)
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

bool SpxMixer::Mixin(const SpxBytecode& spirXBytecode, const string& shaderName, vector<string>& msgs)
{
    vector<string> shaders;
    shaders.push_back(shaderName);
    return Mixin(spirXBytecode, shaders, msgs);
}

bool SpxMixer::Mixin(const SpxBytecode& spirXBytecode, const vector<string>& shaders, vector<string>& msgs)
{
    if (spxCompiler == nullptr) spxCompiler = new SpxCompiler();

    if (!spxCompiler->MixWithShadersFromBytecode(spirXBytecode, shaders))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, string("Fail to mix the shaders from bytecode: " + spirXBytecode.GetName()));
    }

    return true;
}

bool SpxMixer::AddComposition(const string& shaderName, const string& variableName, SpxMixer* mixerSource, vector<string>& msgs)
{
    if (spxCompiler == nullptr) {
        return error(msgs, "mixer is empty");
    }
    if (mixerSource == nullptr || mixerSource->spxCompiler == nullptr) {
        return error(msgs, "mixerSource is null");
    }
    if (shaderName.size() == 0) {
        return error(msgs, "shaderName is invalid");
    }
    if (variableName.size() == 0) {
        return error(msgs, "variableName is invalid");
    }

    if (!spxCompiler->AddComposition(shaderName, variableName, mixerSource->spxCompiler, msgs))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, "Failed to add the composition to the mixer");
    }
    return true;
}

bool SpxMixer::GetCurrentMixinBytecode(SpxBytecode& output, vector<string>& messages)
{
    if (spxCompiler == nullptr)
        return error(messages, "you must process some mixin first");

    spxCompiler->GetMixinBytecode(output.getWritableBytecodeStream());

    return true;
}

bool SpxMixer::Compile(vector<OutputStageBytecode>& outputStages, vector<string>& messages,
    SpvBytecode* composedSpv, SpvBytecode* streamsMergeSpv, SpvBytecode* streamsReshuffledSpv, SpvBytecode* mergedCBuffersSpv, SpvBytecode* compiledBytecode, SpvBytecode* errorLatestSpv)
{
    if (spxCompiler == nullptr)
        return error(messages, "you must process some mixin first");

    if (outputStages.size() == 0)
        return error(messages, "no output stages defined");

    //===================================================================================================================
    //put the output stages in the correct order
    vector<ShadingStageEnum> stagePipeline;
    vector<XkslMixerOutputStage> vecMixerOutputStages;
    SpxCompiler::GetStagesPipeline(stagePipeline);
    for (unsigned int iStage=0; iStage < stagePipeline.size(); ++iStage)
    {
        ShadingStageEnum stage = stagePipeline[iStage];
        for (unsigned int io = 0; io < outputStages.size(); ++io)
        {
            bool stageFound = false; //to check that we don t have 2 outputs for a same stage
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
    SpxCompiler* clonedSpxStream = spxCompiler->Clone();
    if (clonedSpxStream == nullptr) return error(messages, "Failed to clone the spxCompiler");

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
    SpxCompiler::TypeStructMemberArray globalListOfMergedStreamVariables;
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
    if (!clonedSpxStream->ProcessCBuffers(vecMixerOutputStages))
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
    // (remove unused shaders (shader whose methods or members are never called by the output stages))
	// remove all unused functions and members
    if (!clonedSpxStream->RemoveAllUnusedFunctionsAndMembers(vecMixerOutputStages))
    {
        clonedSpxStream->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
        delete clonedSpxStream;
        return error(messages, "Fail to remove all unused stuff");
    }

    //===================================================================================================================
    // Finalize the compilation
    //===================================================================================================================
    if (!clonedSpxStream->FinalizeCompilation(vecMixerOutputStages))
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

    if (compiledBytecode != nullptr)
        clonedSpxStream->GetMixinBytecode(compiledBytecode->getWritableBytecodeStream());

    //===================================================================================================================
    //===================================================================================================================
    //Reflection: get all reflection information from the final extended SPV bytecode
    //EffectReflection effectReflection;
    //if (!clonedSpxStream->GetBytecodeReflectionData(effectReflection))
    //{
    //    clonedSpxStream->copyMessagesTo(messages);
    //    if (errorLatestSpv != nullptr) clonedSpxStream->GetMixinBytecode(errorLatestSpv->getWritableBytecodeStream());
    //    delete clonedSpxStream;
    //    return error(messages, "Fail to build the reflection data from the extended SPV bytecode");
    //}
    //===================================================================================================================

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

bool SpxMixer::GetCompiledBytecodeReflection(SpvBytecode& compiledBytecode, EffectReflection& effectReflection, vector<string>& errorMessages)
{
    SpxCompiler spxCompiler;
    if (!spxCompiler.SetBytecode(compiledBytecode))
    {
        spxCompiler.copyMessagesTo(errorMessages);
        return error(errorMessages, "Fail to initialize the mixer with the bytecode");
    }

    if (!spxCompiler.BuildAllMaps())
    {
        spxCompiler.copyMessagesTo(errorMessages);
        return error(errorMessages, "Fail to build all maps");
    }

    if (!spxCompiler.GetBytecodeReflectionData(effectReflection))
    {
        spxCompiler.copyMessagesTo(errorMessages);
        return error(errorMessages, "Fail to build the reflection data from the bytecode");
    }

    return true;
}
