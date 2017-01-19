//
// Copyright (C) 

#include <cassert>

#include "Utils.h"
#include "XkslangTest.h"

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

using namespace std;
using namespace xkslangtest;

/*
bool XkslangTest::loadFileCompileAndCheck(const string& testDir,
    const string& testName,
    Source source,
    Semantics semantics,
    Target target,
    const string& entryPointName)
{
    const string inputFname = testDir + "/" + testName;
    const string expectedOutputFname = testDir + "/baseResults/" + testName + ".out";
    string xkslInput;

    if (!Utils::ReadFile(inputFname, xkslInput))
    {
        return false;
    }

    const EShMessages controls = Utils::DeriveOptions(source, semantics, target);
    GlslangResult result = compileAndLink(testName, xkslInput, entryPointName, controls);

    // Generate the hybrid output in the way of glslangValidator.
    ostringstream stream;
    Utils::OutputResultToStream(&stream, result, controls);

    // Write the stream output on the disk
    const string newOutputFname = testDir + "/" + testName + ".latest.spv";
    Utils::WriteFile(newOutputFname, stream.str());

    //Compare with an expected output?
    ///string expectedOutput;
    ///ReadFile(expectedOutputFname, expectedOutput);
    ///checkEqAndUpdateIfRequested(expectedOutput, stream.str(), expectedOutputFname);

    return result.success;
}

GlslangResult XkslangTest::compileAndLink(
    const string shaderName, const string& code,
    const string& entryPointName, EShMessages controls,
    bool flattenUniformArrays)
{
    const EShLanguage kind = Utils::GetShaderStage(Utils::GetSuffix(shaderName));

    glslang::TShader shader(kind);
    shader.setFlattenUniformArrays(flattenUniformArrays);

    bool success = compile(&shader, code, entryPointName, controls);

    glslang::TProgram program;
    program.addShader(&shader);
    success &= program.link(controls);

    spv::SpvBuildLogger logger;

    if (success && (controls & EShMsgSpvRules)) {
        vector<uint32_t> spirv_binary;
        glslang::GlslangToSpv(*program.getIntermediate(kind),
            spirv_binary, &logger);

        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, spirv_binary);
        return{ { { shaderName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
            program.getInfoLog(), program.getInfoDebugLog(),
            logger.getAllMessages(), disassembly_stream.str(), true };
    }
    else {
        return{ { { shaderName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
            program.getInfoLog(), program.getInfoDebugLog(), "", "", false };
    }
}

bool XkslangTest::compile(glslang::TShader* shader, const string& code,
    const string& entryPointName, EShMessages controls,
    const TBuiltInResource* resources)
{
    const char* shaderStrings = code.data();
    const int shaderLengths = static_cast<int>(code.size());

    shader->setStringsWithLengths(&shaderStrings, &shaderLengths, 1);
    if (!entryPointName.empty()) shader->setEntryPoint(entryPointName.c_str());
    return shader->parse(
        (resources ? resources : &glslang::DefaultTBuiltInResource),
        defaultVersion, isForwardCompatible, controls);
}
*/