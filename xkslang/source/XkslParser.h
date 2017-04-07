//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <string>
#include <vector>

//#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "define.h"
#include "SpxBytecode.h"

namespace xkslang
{

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

    //bool ConvertShaderToSpirX(const std::string shaderName, std::vector<std::string> listAllExistingShader);

    //Convert a xksl shader into a SPX file
    //The shader string has to contain the shader and all its dependencies
    bool ConvertXkslToSpx(const std::string& shaderFileName, const std::string& shaderString, const std::vector<ShaderGenericsValue>& listGenericsValue, SpxBytecode& spirXBytecode,
        std::ostringstream* errorAndDebugMessages , std::ostringstream* outputHumanReadableASTAndSPV);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLPARSER_H
