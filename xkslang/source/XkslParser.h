//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <string>
#include <vector>

#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "define.h"
#include "SpxBytecode.h"

namespace xkslang
{

//typedef bool(*CallbackRequestDataForShader)(const std::string&, std::string&);

class ShaderGenericsValue
{
public:
    std::string shaderName;
    std::vector<std::string> genericsValue;
};

class XkslParser
{
private:

public:
    XkslParser();
    ~XkslParser();

    bool InitialiseXkslang();
    void Finalize();

    //Recursively convert a shader into SPX bytecode
    //If the shader misses some dependencies, xkslang will query their data through the callback function
    bool ConvertShaderToSpx(const std::string shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader, const std::vector<ShaderGenericsValue>& listGenericsValue, SpxBytecode& spirXBytecode,
        std::ostringstream* errorAndDebugMessages, std::ostringstream* outputHumanReadableASTAndSPV);

    //Convert a xksl file into a SPX bytecode
    //The shader string has to contain the shader and all its dependencies
    bool ConvertXkslFileToSpx(const std::string& shaderFileName, const std::string& data, const std::vector<ShaderGenericsValue>& listGenericsValue, SpxBytecode& spirXBytecode,
        std::ostringstream* errorAndDebugMessages , std::ostringstream* outputHumanReadableASTAndSPV);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLPARSER_H
