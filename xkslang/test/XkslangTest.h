//
// Copyright (C) 

#ifndef XKSLANG_TEST_H
#define XKSLANG_TEST_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <tuple>
#include <string>
#include <vector>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "define.h"

/*
namespace xkslangtest
{
class XkslangTest
{
private:
    const int defaultVersion;
    const EProfile defaultProfile;
    const bool forceVersionProfile;
    const bool isForwardCompatible;

public:
    XkslangTest() :
        defaultVersion(100),
        defaultProfile(ENoProfile),
        forceVersionProfile(false),
        isForwardCompatible(false) {}

    bool loadFileCompileAndCheck(const std::string& testDir,
        const std::string& testName,
        Source source,
        Semantics semantics,
        Target target,
        const std::string& entryPointName = "");

    // Compiles and links the given source |code| of the given shader
    // |stage| into the target under the semantics specified via |controls|.
    // Returns a GlslangResult instance containing all the information generated
    // during the process. If the target includes SPIR-V, also disassembles
    // the result and returns disassembly text.
    GlslangResult compileAndLink(
        const std::string shaderName, const std::string& code,
        const std::string& entryPointName, EShMessages controls,
        bool flattenUniformArrays = false);

    // Compiles and the given source |code| of the given shader |stage| into
    // the target under the semantics conveyed via |controls|. Returns true
    // and modifies |shader| on success.
    bool compile(glslang::TShader* shader, const std::string& code,
        const std::string& entryPointName, EShMessages controls,
        const TBuiltInResource* resources = nullptr);
};

}  // namespace glslangtest
*/

#endif  // XKSLANG_TEST_H
