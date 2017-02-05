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
    assert(!isInitialized);

    bool res = glslang::InitializeProcess();
    isInitialized = res;
    return res;
}

void XkslParser::Finalize()
{
    isInitialized = false;
    glslang::FinalizeProcess();
}

bool XkslParser::ConvertXkslToSpirX(const string& shaderFileName, const string& shaderString, SpxBytecode& spirXBytecode,
    std::ostringstream* errorAndDebugMessages, std::ostringstream* outputHumanReadableASTAndSPV)
{
    const char* shaderStrings = shaderString.data();
    const int shaderLengths = static_cast<int>(shaderString.size());

    const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    bool flattenUniformArrays = false;
    TBuiltInResource* resources = nullptr;
    bool buildSPRV = true;
    bool keepUncalledFuntionsInAST = true;

    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules);
    if (keepUncalledFuntionsInAST) controls = static_cast<EShMessages>(controls | EShMsgKeepUncalled);
    if (buildSPRV) controls = static_cast<EShMessages>(controls | EShMsgSpvRules);

    glslang::TShader shader(kind);
    shader.setFlattenUniformArrays(flattenUniformArrays);

    shader.setStringsWithLengths(&shaderStrings, &shaderLengths, 1);
    //shader.setEntryPoint(entryPointName.c_str());

    bool success = false;

    success = shader.parseXkslShaderFile(shaderFileName, (resources ? resources : &glslang::DefaultTBuiltInResource), controls);

    //We don't really need to link, but glslang will do some final checkups, then give access to the intermediary
    glslang::TProgram program;
    program.addShader(&shader);
    success &= program.link(controls);
    glslang::TIntermediate* AST = program.getIntermediate(kind);

    vector<uint32_t> bytecodeList;
    spv::SpvBuildLogger logger;

    //=============================================================================================
    //=============================================================================================
    //Build the SPRX bytecode from the AST
    if (success && AST != nullptr)
    {
        glslang::GlslangToSpv(*AST, bytecodeList, &logger);
        spirXBytecode.clear();
        spirXBytecode.SetBytecode(bytecodeList);
        spirXBytecode.SetName(shaderFileName);
        if (logger.hasAnyError()) success = false;
    }
    
    //output debug and error messages
    if (errorAndDebugMessages != nullptr)
    {
        const char* c;
        ostringstream& stream = *errorAndDebugMessages;

        stream << shaderFileName << "\n";
        c = shader.getInfoLog();
        if (c != nullptr && strlen(c) > 0) stream << c << "\n";
        stream << logger.getAllMessages();
        stream << "\n";
    }

    //output Human Readable form of AST and SPIV bytecode
    if (outputHumanReadableASTAndSPV != nullptr)
    {    
        const char* c;
        ostringstream& stream = *outputHumanReadableASTAndSPV;

        c = shader.getInfoDebugLog();
        if (c != nullptr && strlen(c) > 0) stream << c << "\n";
        
        if (success && AST != nullptr)
        {
            //dissassemble the binary
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, bytecodeList);

            stream << disassembly_stream.str();
        }
    }

    return success;
}

