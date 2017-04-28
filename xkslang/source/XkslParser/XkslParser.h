//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <string>
#include <vector>
#include <sstream>

#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "../Common/define.h"
#include "../Common/SpxBytecode.h"

namespace xkslang
{

//typedef bool(*CallbackRequestDataForShader)(const std::string&, std::string&);

class GenericValue
{
public:
    std::string label;
    std::string value;

    GenericValue(std::string label, std::string value) : label(label), value(value) {}
};

class ShaderGenericValues
{
public:
    std::string shaderName;
    std::vector<GenericValue> genericsValue;
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
    bool ConvertShaderToSpx(const std::string shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader, const std::vector<ShaderGenericValues>& listGenericsValue,
        SpxBytecode& spirXBytecode, std::ostringstream* errorAndDebugMessages);

    bool ConvertBytecodeToText(const std::vector<uint32_t>& bytecode, std::string& text);

    //Convert a xksl file into a SPX bytecode
    //The shader string has to contain the shader and all its dependencies
    bool ConvertXkslFileToSpx(const std::string& shaderFileName, const std::string& data, const std::vector<ShaderGenericValues>& listGenericsValue, SpxBytecode& spirXBytecode,
        std::ostringstream* errorAndDebugMessages);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLPARSER_H
