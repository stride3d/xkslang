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

#include "XkslParser.h"

//TMP
#include "../test/Utils.h"
#include "../test/define.h"

using namespace std;
using namespace xkslparser;

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

bool XkslParser::ParseXkslShader(const std::string& shaderFileName, const std::string& shaderString)
{
    const char* shaderStrings = shaderString.data();
    const int shaderLengths = static_cast<int>(shaderString.size());

    //TMP  TODO: check the impact of these parameters
    const EShLanguage kind = EShLangFragment;
    bool flattenUniformArrays = false;
    bool isForwardCompatible = false;
    int defaultVersion = 100;
    TBuiltInResource* resources = nullptr;
    bool buildSPRV = true;
    bool keepUncalledFuntionsInAST = true;

    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules | EShMsgKeepUncalled);
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

    //======================================================================
    //TEMPORARY: link, build SPRV and save to file
    {
        glslang::TProgram program;
        program.addShader(&shader);
        success &= program.link(controls);
    
        spv::SpvBuildLogger logger;
    
        xkslangtest::GlslangResult glslangRes;
        if (success && (controls & EShMsgSpvRules)) {
            vector<uint32_t> spirv_binary;
            glslang::GlslangToSpv(*program.getIntermediate(kind), spirv_binary, &logger);
    
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, spirv_binary);
            glslangRes = xkslangtest::GlslangResult{ { { shaderFileName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
                program.getInfoLog(), program.getInfoDebugLog(),
                logger.getAllMessages(), disassembly_stream.str(), true };
        }
        else {
            glslangRes = xkslangtest::GlslangResult{ { { shaderFileName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
                program.getInfoLog(), program.getInfoDebugLog(), "", "", false };
        }
    
        //Write result to file
        // Generate the hybrid output in the way of glslangValidator.
        ostringstream stream;
        xkslangtest::Utils::OutputResultToStream(&stream, glslangRes, controls);
    
        std::string testDir = "D:/Prgms/glslang/source/Test/xksl";
    
        // Write the stream output on the disk
        const string newOutputFname = testDir + "/" + shaderFileName + ".latest.spv";
        xkslangtest::Utils::WriteFile(newOutputFname, stream.str());
    }

    return success;
}