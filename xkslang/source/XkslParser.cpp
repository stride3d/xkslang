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

bool XkslParser::ConvertXkslToSpirX(const std::string& shaderFileName, const std::string& shaderString, SPXBytecode& spirXBytecode)
{
    const char* shaderStrings = shaderString.data();
    const int shaderLengths = static_cast<int>(shaderString.size());

    const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    bool flattenUniformArrays = false;
    bool isForwardCompatible = false;
    int defaultVersion = 100;
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

    success = shader.parseXkslShaderFile(shaderFileName,
        (resources ? resources : &glslang::DefaultTBuiltInResource),
        defaultVersion, isForwardCompatible, controls);

    //We don't really need to link, but glslang will do some final checkups and give access to the intermediary
    glslang::TProgram program;
    program.addShader(&shader);
    success &= program.link(controls);
    glslang::TIntermediate* AST = program.getIntermediate(kind);

    std::vector<uint32_t> bytecodeList;
    spv::SpvBuildLogger logger;

    //======================================================================
    //build SPRV
    if (success && AST != nullptr)
    {
        glslang::GlslangToSpv(*AST, bytecodeList, &logger);
        spirXBytecode.clear();
        spirXBytecode.SetBytecode(bytecodeList);
    }
    
    //TEMPORARY: output stuff
    {
        std::string testDir = "D:/Prgms/glslang/source/Test/xksl";

        //output SPIRV binary
        if (bytecodeList.size() > 0)
        {
            const string newOutputFname = testDir + "/" + shaderFileName + ".spv";
            glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
        }

        xkslangtest::GlslangResult glslangRes;
        if (success && AST != nullptr)
        {
            //dissassemble the binary
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, bytecodeList);
            glslangRes = xkslangtest::GlslangResult{ { { shaderFileName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
                program.getInfoLog(), program.getInfoDebugLog(),
                logger.getAllMessages(), disassembly_stream.str(), true };
        }
        else
        {
            glslangRes = xkslangtest::GlslangResult{ { { shaderFileName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
                program.getInfoLog(), program.getInfoDebugLog(), "", "", false };
        }

        //Output messages, AST and SPIRV into a human readable file
        ostringstream stream;
        xkslangtest::Utils::OutputResultToStream(&stream, glslangRes, controls);

        // Write the stream output on the disk
        //const string filenameWithoutSuffix = xkslangtest::Utils::RemoveSuffix(shaderFileName);
        const string newOutputFname = testDir + "/" + shaderFileName + ".hr.spv";
        xkslangtest::Utils::WriteFile(newOutputFname, stream.str());
    }

    return success;
}

