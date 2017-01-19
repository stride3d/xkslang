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

//SPVFunction* SPVObject::GetAsSPVFunction() { return dynamic_cast<SPVFunction*>(this); }
//SPVShader* SPVObject::GetAsSPVShader() { return dynamic_cast<SPVShader*>(this); }

//=============================================================================================================//
//=============================================================================================================//

XkslMixer::XkslMixer()
{
    spxStreamRemapper = nullptr;
    //listSpxStream.clear();
    m_astGenerated = false;
}

XkslMixer::~XkslMixer()
{
    //for (int i = 0; i < listSpxStream.size(); ++i)
    //{
    //    delete listSpxStream[i];
    //}
    if (spxStreamRemapper != nullptr) delete spxStreamRemapper;
}

void XkslMixer::AddMixin(SpxBytecode* spirXBytecode)
{
    listMixins.push_back(spirXBytecode);
}

//bool XkslMixer::AddSPVFunction(SPVFunction* func)
//{
//    this->listAllFunctions.push_back(func);
//    return true;
//}
//
//bool XkslMixer::AddSPVShader(SPVShader* shader)
//{
//    this->listAllShaders.push_back(shader);
//    return true;
//}
//
//SPVShader* XkslMixer::GetSpvShaderByName(const std::string& name)
//{
//    int count = listAllShaders.size();
//    for (int i = 0; i < count; ++i)
//    {
//        if (listAllShaders[i]->declarationName == name) return listAllShaders[i];
//    }
//    return nullptr;
//}
//
//SPVFunction* XkslMixer::GetFunctionCalledByName(const std::string& name)
//{
//    //TMP: stop at the first function having the specified name
//    int count = listAllFunctions.size();
//    for (int i = 0; i < count; ++i)
//    {
//        if (listAllFunctions[i]->declarationName == name)
//        {
//            SPVFunction* function = listAllFunctions[i];
//            if (function->isOverriddenBy != nullptr) function = function->isOverriddenBy;
//            return function;
//        }
//    }
//    return nullptr;
//}

bool XkslMixer::MergeAllMixin(vector<string>& msgs)
{
    if (listMixins.size() == 0)
        return error(msgs, "No bytecodes in the mixin list");

    //call the function one time only
    if (m_astGenerated || spxStreamRemapper != nullptr)
        return error(msgs, "Mixin has already been created");

    //======================================================================================================
    //======================================================================================================
    // Generate mixins
    spxStreamRemapper = new SpxStreamRemapper();
    for (int mixinNum=0; mixinNum<listMixins.size(); ++mixinNum)
    {
        SpxBytecode* spirXBytecode = listMixins[mixinNum];

        if (!spxStreamRemapper->MixSpxBytecodeStream(*spirXBytecode))
        {
            spxStreamRemapper->copyMessagesTo(msgs);
            return error(msgs, "Fail to mix the bytecode");
        }
    }
    if (!spxStreamRemapper->FinalizeMixin())
    {
        spxStreamRemapper->copyMessagesTo(msgs);
        return error(msgs, "Fail to finalize the mixin");
    }

    /*
    //======================================================================================================
    //======================================================================================================
    // Parse the SPIRX bytecodes (disassemble the bytecode instructions)
    for (int mixinNum = 0; mixinNum<listMixins.size(); ++mixinNum)
    {
        SpxBytecode* spirXBytecode = listMixins[mixinNum];

        //Parse the bytecode (disassemble)
        SpxStreamParser* sprxStream = DisassembleSpxBytecode(spirXBytecode, mixinNum, msgs);
        if (sprxStream == nullptr) {
            sprxStream->copyMessagesTo(msgs);
            return error(msgs, "Fail to parse the list of mixins");
        }
        listSpxStream.push_back(sprxStream);
    }

    //======================================================================================================
    //======================================================================================================
    // Process the parsed bytecodes
    // - Link the classes according the inheritance
    // - Link the functions with their owner class
    // - Add all functions attributes (abstract, override, stage, clone)
    int countSpxStream = listSpxStream.size();
    for (int i = 0; i<countSpxStream; ++i)
    {
        SpxStreamParser* sprxStream = listSpxStream[i];

        //process the bytecode
        bool success = sprxStream->DecorateAllObjects();
        if (!success)
        {
            sprxStream->copyMessagesTo(msgs);
            return error(msgs, "Fail to decorate the spxStream");
        }
    }

    //======================================================================================================
    //======================================================================================================
    // Process methods overrides
    ProcessOverrideMethods();
    */

    m_astGenerated = true;
    return true;
}

/*
void OverrideShaderParentsMethodRecursif(SPVShader* shader, const string& overringMethodName, SPVFunction* overringMethod)
{
    if (shader == nullptr) return;

    for (int p = 0; p < shader->parents.size(); ++p)
    {
        SPVShader* parent = shader->parents[p];
        for (int m = 0; m < parent->methods.size(); m++)
        {
            SPVFunction* aFunc = parent->methods[m];
            if (aFunc->declarationName == overringMethodName)
            {
                aFunc->SetOverridingFunction(overringMethod);
            }
        }

        OverrideShaderParentsMethodRecursif(parent, overringMethodName, overringMethod);
    }
}

void XkslMixer::ProcessOverrideMethods()
{
    int countFunctions = listAllFunctions.size();
    for (int i = 0; i < countFunctions; ++i)
    {
        SPVFunction* func = listAllFunctions[i];

        //is the function overriding another one?
        if (func->isOverride)
        {
            //set to override all functions from parents classes declared with the same mangled name 
            //TMP: a function override only its parents class for now: no mixin, no border case of parent being mixed after children
            
            SPVShader* shaderOwner = func->shaderOwner;
            if (shaderOwner != nullptr)
            {
                OverrideShaderParentsMethodRecursif(shaderOwner, func->declarationName, func);
            }
        }
    }
}
*/

bool XkslMixer::GetMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages)
{
    if (!m_astGenerated || spxStreamRemapper == nullptr)
        return error(messages, "The mixin AST must been created first");

    spxStreamRemapper->GetMappedSpxBytecode(output);

    return true;
}

bool XkslMixer::GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages)
{
    if (!m_astGenerated || spxStreamRemapper == nullptr)
        return error(messages, "The mixin AST must been created first");

    bool success = spxStreamRemapper->GenerateSpvStageBytecode(stage, entryPoint, output);
    if (!success)
    {
        spxStreamRemapper->copyMessagesTo(messages);
        return false;
    }

    return true;

    /*
    //Find entry point function
    SPVFunction* entryPointFunction = GetFunctionCalledByName(entryPoint);
    if (entryPointFunction == nullptr)
        return error(messages, string("Cannot find the entryPoint function in the mixin shaders: ") + entryPoint);

    //======================================================================================================
    //======================================================================================================
    // Build the SPIV bytecode
    SpxMixerToSpvBuilder* builder = new SpxMixerToSpvBuilder();
    builder->SetupBuilder(entryPointFunction, stage);
    
    //Proto: copy all functions
    for (int f = 0; f < listAllFunctions.size(); ++f)
    {
        SPVFunction* func = listAllFunctions[f];
        if (func == entryPointFunction) continue;

        //clone the function code
        SpxStreamParser* sprxStream = func->stream;
        for (int opIndex = func->opStart; opIndex < func->opEnd; ++opIndex)
        {
            //gfdgdsfg;
        }
    }

    builder->FinalizeBuilder();

    builder->DumpToSpvBytecode(bytecode);
    builder->DumpLoggerMessage(msgs);

    delete builder;

    return true;
    */
}

/*
SpxStreamParser* XkslMixer::DisassembleSpxBytecode(SpxBytecode* spirXBytecode, int streamMixinNum, vector<string>& msgs)
{
    const vector<uint32_t>& stream = spirXBytecode->getBytecodeStream();

    SpxStreamParser* streamParser = new SpxStreamParser(this, stream);

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
    //Iterate through SPX instructions
    bool success = streamParser->DisassembleSpirXStream(streamMixinNum);
    if (!success)
    {
        streamParser->copyMessagesTo(msgs);
        delete streamParser; return nullptr;
    }

    return streamParser;
}
*/