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
    status = MixerStatusEnum::WaitingForMixin;
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
    if (status != MixerStatusEnum::WaitingForMixin) return error(msgs, "The mixer has an invalid status");
    if (spxCompiler == nullptr) spxCompiler = new SpxCompiler();

    if (!spxCompiler->MixWithShadersFromBytecode(spirXBytecode, shaders))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, string("Fail to mix the shaders from bytecode: " + spirXBytecode.GetName()));
    }

    return true;
}

bool SpxMixer::GetListAllMethodsInfo(vector<MethodInfo>& vecMethods, vector<string>& msgs)
{
    if (spxCompiler == nullptr) {
        return error(msgs, "mixer is empty");
    }

    if (!spxCompiler->GetListAllMethodsInfo(vecMethods))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, "Failed to get the list of all method infos from the mixer");
    }
    return true;
}

bool SpxMixer::GetListAllCompositionsInfo(vector<ShaderCompositionInfo>& vecCompositions, vector<string>& msgs)
{
    if (spxCompiler == nullptr) {
        return error(msgs, "mixer is empty");
    }

    if (!spxCompiler->GetListAllCompositionsInfo(vecCompositions))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, "Failed to get the list of all composition infos from the mixer");
    }
    return true;
}

bool SpxMixer::AddCompositionInstance(const string& shaderName, const string& variableName, SpxMixer* mixerSource, vector<string>& msgs)
{
    if (status != MixerStatusEnum::WaitingForMixin) return error(msgs, "The mixer has an invalid status");

    if (spxCompiler == nullptr) {
        return error(msgs, "mixer is empty");
    }
    if (mixerSource == nullptr || mixerSource->spxCompiler == nullptr) {
        return error(msgs, "mixerSource is null");
    }
    if (variableName.size() == 0) {
        return error(msgs, "variableName is invalid");
    }

    if (!spxCompiler->AddCompositionInstance(shaderName, variableName, mixerSource->spxCompiler))
    {
        spxCompiler->copyMessagesTo(msgs);
        return error(msgs, "Failed to add the composition to the mixer");
    }
    return true;
}

const vector<uint32_t>* SpxMixer::GetCurrentMixinBytecode()
{
    if (spxCompiler == nullptr) return nullptr;
    return &(spxCompiler->GetMixinBytecode());
}

bool SpxMixer::CopyCurrentMixinBytecode(vector<uint32_t>& bytecode, vector<string>& messages)
{
    if (spxCompiler == nullptr)
        return error(messages, "you must process some mixin first");

    spxCompiler->CopyMixinBytecode(bytecode);
    return true;
}

bool SpxMixer::Compile(vector<OutputStageBytecode>& outputStages, vector<string>& messages,
    vector<uint32_t>* composedSpv, vector<uint32_t>* streamsMergeSpv, vector<uint32_t>* streamsReshuffledSpv, vector<uint32_t>* mergedCBuffersSpv,
    vector<uint32_t>* compiledBytecode, vector<uint32_t>* errorLatestSpv)
{
    if (spxCompiler == nullptr) return error(messages, "you must process some mixin first");
    if (status == MixerStatusEnum::Compiled) return error(messages, "The mixer has already been compiled");
    status = MixerStatusEnum::Compiled;

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
    ///SpxCompiler* clonedSpxStream = spxCompiler->Clone();
    ///if (clonedSpxStream == nullptr)
    ///{
    ///    spxCompiler->copyMessagesTo(messages);
    ///    return error(messages, "Failed to clone the spxCompiler");
    ///}

    //===================================================================================================================
    //===================================================================================================================
    // initialize the compilation process (validate the bytecode, search for the output stage entry point functions, ...)
    if (!spxCompiler->InitializeCompilationProcess(vecMixerOutputStages))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Failed to initialize the compilation process");
    }

    //===================================================================================================================
    // Update overriding methods
    //===================================================================================================================
    //retarget the call to OpFunction instruction according to the overriding functions
    if (!spxCompiler->UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions())
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Remapping overriding functions failed");
    }

    //===================================================================================================================
    // Process compositions
    //===================================================================================================================
    // apply all composition instances
    if (!spxCompiler->ApplyCompositionInstancesToBytecode())
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Failed to apply all compositions to the bytecode");
    }

    if (composedSpv != nullptr) spxCompiler->CopyMixinBytecode(*composedSpv);

    //===================================================================================================================
    // Process streams
    //===================================================================================================================
    // merge all stream variables into a global single struct
    SpxCompiler::TypeStructMemberArray globalListOfMergedStreamVariables;
    if (!spxCompiler->MergeStreamMembers(globalListOfMergedStreamVariables))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to merge the streams");
    }

    if (streamsMergeSpv != nullptr) spxCompiler->CopyMixinBytecode(*streamsMergeSpv);

    //===================================================================================================================
    // analyse the stream and cbuffers usage for each stage
    if (!spxCompiler->AnalyseStreamsAndCBuffersAccessesForOutputStages(vecMixerOutputStages, globalListOfMergedStreamVariables))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Analysing the streams failed");
    }

    /*if (vecMixerOutputStages.size() == 1 && (vecMixerOutputStages[0].outputStage->stage == ShadingStageEnum::Vertex || vecMixerOutputStages[0].outputStage->stage == ShadingStageEnum::Pixel))
    {
        //this would normally returns an error in the streams validation, but we just skip this step for now to allow compilation of partial sample effects
    }
    else*/
    {
        //validate conformity with stream flow
        if (!spxCompiler->ValidateStagesStreamMembersFlow(vecMixerOutputStages, globalListOfMergedStreamVariables))
        {
            spxCompiler->copyMessagesTo(messages);
            if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
            return error(messages, "Validating the stages stream members flow failed");
        }

        //===================================================================================================================
        //===================================================================================================================
        // reshuffle the stream members
        if (!spxCompiler->ReshuffleStreamVariables(vecMixerOutputStages, globalListOfMergedStreamVariables))
        {
            spxCompiler->copyMessagesTo(messages);
            if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
            return error(messages, "Fail to reshuffle the streams");
        }

        if (streamsReshuffledSpv != nullptr)
            spxCompiler->CopyMixinBytecode(*streamsReshuffledSpv);
    }

    //===================================================================================================================
    // Process cbuffers
    //===================================================================================================================
    // remove unused cbuffers, merge used cbuffers havind same name
    if (!spxCompiler->ProcessCBuffers(vecMixerOutputStages))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to process the cbuffers");
    }
    if (mergedCBuffersSpv != nullptr)
        spxCompiler->CopyMixinBytecode(*mergedCBuffersSpv);

    //===================================================================================================================
    //===================================================================================================================
	// remove all unused functions and members
    if (!spxCompiler->RemoveAllUnusedFunctionsAndMembers(vecMixerOutputStages))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to remove all unused stuff");
    }

    //===================================================================================================================
    // Finalize the compilation
    //===================================================================================================================
    if (!spxCompiler->FinalizeCompilation(vecMixerOutputStages))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to finalize the compilation");
    }

#ifdef XKSLANG_DEBUG_MODE
    //Before final compilation step: do a full sanity check on the bytecode (useful in debug to help detecting problems earlier)
    if (!spxCompiler->ProcessFullBytecodeAndDataSanityCheck())
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to process a bytecode and data sanity check");
    }
#endif

    if (compiledBytecode != nullptr)
        spxCompiler->CopyMixinBytecode(*compiledBytecode);

    //===================================================================================================================
    // Build SPV bytecode for each output stages
    //===================================================================================================================
    if (!spxCompiler->GenerateBytecodeForAllStages(vecMixerOutputStages))
    {
        spxCompiler->copyMessagesTo(messages);
        if (errorLatestSpv != nullptr) spxCompiler->CopyMixinBytecode(*errorLatestSpv);
        return error(messages, "Fail to finalize the mixin");
    }

    //=============================================================================================================================================
    //=============================================================================================================================================

    return true;
}

bool SpxMixer::GetCompiledBytecodeReflection(std::vector<uint32_t>& compiledBytecode, EffectReflection& effectReflection, vector<string>& errorMessages)
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
