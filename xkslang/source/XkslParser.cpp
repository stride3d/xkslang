//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "glslang/MachineIndependent/localintermediate.h"
//#include "glslang/include/intermediate.h"
#include "StandAlone/ResourceLimits.h"

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

#include "XkslParser.h"

//TMP
#include "../test/Utils.h"
#include "../test/define.h"

using namespace std;
using namespace xkslang;

XkslParser::XkslParser()
{}

XkslParser::~XkslParser()
{}

static bool isInitialized = false;
bool XkslParser::InitialiseXkslang()
{
    if (isInitialized) return true;

    bool res = glslang::InitializeProcess();
    isInitialized = res;
    return res;
}

void XkslParser::Finalize()
{
    isInitialized = false;
    glslang::FinalizeProcess();
}

//Convert a xksl file into a SPX bytecode
//The shader string has to contain the shader and all its dependencies
bool XkslParser::ConvertXkslFileToSpx(const string& shaderFileName, const string& data, const vector<ShaderGenericsValue>& listGenericsValue, SpxBytecode& spirXBytecode,
    std::ostringstream* errorAndDebugMessages, std::ostringstream* outputHumanReadableASTAndSPV)
{
    if (!isInitialized){
        if (errorAndDebugMessages != nullptr) (*errorAndDebugMessages) << "xkslParser has not been initialized, call XkslParser::InitialiseXkslang() function first" << endl;
        return false;
    }
    bool success = false;

    //const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    const TBuiltInResource& resources = glslang::DefaultTBuiltInResource;
    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules);

    //Convert ShaderGenericsValue to ClassGenericsValue
    vector<ClassGenericsValue> listGenericsPerShader;
    for (unsigned int k = 0; k < listGenericsValue.size(); ++k)
    {
        listGenericsPerShader.push_back(ClassGenericsValue());
        ClassGenericsValue& sg = listGenericsPerShader.back();
        sg.targetName = listGenericsValue[k].shaderName;
        sg.genericsValue = listGenericsValue[k].genericsValue;
    }

    vector<uint32_t>& spxBytecode = spirXBytecode.getWritableBytecodeStream();
    vector<string> errorMsgs;
    success = glslang::ConvertXkslFileToSpx(shaderFileName, data, listGenericsPerShader, &resources, controls, spxBytecode, errorMsgs);

    //output debug and error messages
    if (errorAndDebugMessages != nullptr)
    {
        ostringstream& stream = *errorAndDebugMessages;

        stream << shaderFileName << "\n";
        for (unsigned int i = 0; i < errorMsgs.size(); ++i)
            stream << errorMsgs[i] << endl;
        stream << "\n";
    }

    //output Human Readable form of AST and SPIV bytecode
    if (success && outputHumanReadableASTAndSPV != nullptr)
    {    
        ostringstream& stream = *outputHumanReadableASTAndSPV;
        
        //dissassemble the binary
        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, spxBytecode);
        stream << disassembly_stream.str();
    }

    return success;
}

//Recursively convert a shader into SPX bytecode
//If the shader requires some dependencies, xkslang will query their data through the callback function
bool XkslParser::ConvertShaderToSpx(const std::string shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader, const std::vector<ShaderGenericsValue>& listGenericsValue, SpxBytecode& spirXBytecode,
    std::ostringstream* errorAndDebugMessages, std::ostringstream* outputHumanReadableASTAndSPV)
{
    if (!isInitialized) {
        if (errorAndDebugMessages != nullptr) (*errorAndDebugMessages) << "xkslParser has not been initialized, call XkslParser::InitialiseXkslang() function first" << endl;
        return false;
    }
    bool success = true;

    //const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    const TBuiltInResource& resources = glslang::DefaultTBuiltInResource;
    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules);

    //Convert ShaderGenericsValue to ClassGenericsValue
    vector<ClassGenericsValue> listGenericsPerShader;
    for (unsigned int k = 0; k < listGenericsValue.size(); ++k)
    {
        listGenericsPerShader.push_back(ClassGenericsValue());
        ClassGenericsValue& sg = listGenericsPerShader.back();
        sg.targetName = listGenericsValue[k].shaderName;
        sg.genericsValue = listGenericsValue[k].genericsValue;
    }

    vector<uint32_t>& spxBytecode = spirXBytecode.getWritableBytecodeStream();
    vector<string> errorMsgs;
    success = glslang::ConvertXkslShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsPerShader, &resources, controls, spxBytecode, errorMsgs);

    //output debug and error messages
    if (errorAndDebugMessages != nullptr)
    {
        ostringstream& stream = *errorAndDebugMessages;

        for (unsigned int i = 0; i < errorMsgs.size(); ++i)
            stream << errorMsgs[i] << endl;
    }

    //output Human Readable form of AST and SPIV bytecode
    if (success && outputHumanReadableASTAndSPV != nullptr)
    {
        ostringstream& stream = *outputHumanReadableASTAndSPV;

        //dissassemble the binary
        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, spxBytecode);
        stream << disassembly_stream.str();
    }

    return success;
}

